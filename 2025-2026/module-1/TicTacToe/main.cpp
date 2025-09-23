#include <cstdint>
#include <bits/stdc++.h>
using namespace std;

// ============================================================
// RPS constants and utilities
// ============================================================

enum Move { ROCK = 0, SCISSORS = 1, PAPER = 2 };
static const char* MOVE_NAME[3] = {"K(камень)","N(ножницы)","B(бумага)"};
static const char* MOVE_CHAR[3] = {"K","N","B"};

inline int counter_to(int m) { // move that beats m
    // ROCK(0)->PAPER(2), SCISSORS(1)->ROCK(0), PAPER(2)->SCISSORS(1)
    return (m + 2) % 3;
}
inline int loses_to(int m) { // move that m beats
    return (m + 1) % 3;
}
inline int result_ai(int ai, int pl) {
    if (ai == pl) return 0;
    // ai beats pl?
    if ((ai == ROCK && pl == SCISSORS) ||
        (ai == SCISSORS && pl == PAPER) ||
        (ai == PAPER && pl == ROCK)) return 1;
    return -1;
}

inline int parse_move(const string& s) {
    // supports k/n/b and Cyrillic к/н/б, case-insensitive
    if (s.empty()) return -1;
    unsigned char c = (unsigned char)tolower(s[0]);
    // Latin
    if (c=='k') return ROCK;
    if (c=='n') return SCISSORS;
    if (c=='b') return PAPER;
    // Cyrillic (UTF-8 naive check by second byte possible), do simple fallback:
    // Accept ASCII mapping for simplicity: assume lowercase 'k','n','b' equivalent.
    // If terminal passes Cyrillic, often first byte is multi-byte; we handle basic Latin only robustly.
    return -1;
}

inline string move_to_str(int m) {
    if (m<0 || m>2) return "?";
    return MOVE_NAME[m];
}
inline string move_to_char(int m) {
    if (m<0 || m>2) return "?";
    return MOVE_CHAR[m];
}

// RNG helper
struct RNG {
    std::mt19937 gen;
    explicit RNG(uint32_t seed=42u): gen(seed) {}
    int randint(int lo,int hi){ std::uniform_int_distribution<int> d(lo,hi); return d(gen); }
    double randu(){ std::uniform_real_distribution<double> d(0.0,1.0); return d(gen); }
    int sample_categorical(const vector<double>& p) {
        double s=0; for(double v:p) s+=v;
        double r = randu()*s;
        double acc=0;
        for(int i=0;i<(int)p.size();++i){ acc+=p[i]; if(r<=acc) return i; }
        return (int)p.size()-1;
    }
    int sample_move_uniform(){ return randint(0,2); }
    int sample_move_biased(int fav, double pFav){
        double p[3] = {(1-pFav)/2.0,(1-pFav)/2.0,(1-pFav)/2.0};
        p[fav] = pFav;
        double r = randu();
        double acc=0;
        for(int i=0;i<3;++i){ acc+=p[i]; if(r<=acc) return i; }
        return 2;
    }
};

// ============================================================
// Archetypes (opponents)
// ============================================================
// 12 archetypes:
// 0) Uniform random
// 1) Biased random (bias to one move)
// 2) Win-Stay / Lose-Shift
// 3) Lose-Stay / Win-Shift
// 4) Cyclic (forward or reverse, chosen per episode)
// 5) Markov (1st order) with 3x3 matrix
// 6) Copying (plays your previous move)
// 7) Anti-copying (plays counter to your previous move)
// 8) Streaky: repeats with prob p, resets after loss
// 9) Tilt: after 2 losses biases to one move
// 10) Gambler: avoids repeats
// 11) Tie-reactive: after draw switches by rule

struct EpisodeHistory {
    vector<int> ai;
    vector<int> pl;
    vector<int> res; // from AI perspective: 1 win, 0 draw, -1 lose
};

struct Opponent {
    int type; // 0..11
    int T=10;
    // internal params/state
    int favMove=0; double favProb=0.7;      // biased
    int cycleDir=1; int cycleCur=0;         // cyclic
    double markov[3][3]{}; int markovCur=0; // markov
    int streakCur=0; double p_repeat=0.72;  // streaky
    int tiltMove=0; int loseStreak=0; double tiltBias=0.85; // tilt
    double repeat_penalty=0.12;             // gambler
    int last_tie_move=-1;                   // tie-reactive

    Opponent(int t, RNG& rng): type(t) {
        // randomize params per episode
        if (t==1) { // biased
            favMove = rng.randint(0,2);
            favProb = 0.6 + 0.2*rng.randu(); // 0.6..0.8
        } else if (t==4) { // cyclic
            cycleDir = (rng.randu()<0.5? +1 : -1);
            cycleCur = rng.randint(0,2);
        } else if (t==5) { // markov
            // A slightly asymmetric matrix; rows sum to 1
            double M[3][3] = {
                {0.15, 0.55, 0.30},
                {0.35, 0.25, 0.40},
                {0.45, 0.30, 0.25}
            };
            // small noise
            for(int i=0;i<3;++i){
                double sum=0;
                for(int j=0;j<3;++j){
                    double eps=(rng.randu()-0.5)*0.05;
                    markov[i][j]=max(0.01, M[i][j]+eps);
                    sum+=markov[i][j];
                }
                for(int j=0;j<3;++j) markov[i][j]/=sum;
            }
            markovCur = rng.randint(0,2);
        } else if (t==8) {
            p_repeat = 0.65 + 0.2*rng.randu();
            streakCur = rng.randint(0,2);
        } else if (t==9) {
            tiltMove = rng.randint(0,2);
            tiltBias = 0.8 + 0.15*rng.randu();
        } else if (t==10) {
            repeat_penalty = 0.05 + 0.15*rng.randu(); // prefer not to repeat last with prob ~0.85-0.95
        }
    }

    int next_move(const EpisodeHistory& H, RNG& rng) {
        int t = (int)H.pl.size(); // next step index
        int ai_last = (t==0? rng.randint(0,2) : H.ai.back());
        int pl_last = (t==0? rng.randint(0,2) : H.pl.back());
        int res_last = (t==0? 0 : H.res.back());
        // count lose streak (player loses -> AI win)
        if (t==0) loseStreak = 0;
        else {
            if (res_last==1) loseStreak++; else if(res_last==-1) loseStreak=0; // player's loss increments
        }

        switch(type){
            case 0: { // uniform random
                return rng.sample_move_uniform();
            }
            case 1: { // biased random
                return rng.sample_move_biased(favMove, favProb);
            }
            case 2: { // Win-Stay / Lose-Shift
                if (t==0) return rng.sample_move_uniform();
                if (res_last==-1) { // AI lost -> player won -> Win-Stay (repeat)
                    return pl_last;
                } else if (res_last==1) { // AI win -> player lost -> Lose-Shift (counter AI last)
                    return counter_to(ai_last);
                } else { // draw -> mild shift
                    // change to counter of own last with prob .5 else repeat
                    if (rng.randu()<0.5) return counter_to(pl_last);
                    return pl_last;
                }
            }
            case 3: { // Lose-Stay / Win-Shift
                if (t==0) return rng.sample_move_uniform();
                if (res_last==1) { // player lost -> Lose-Stay
                    return pl_last;
                } else if (res_last==-1) { // player won -> Win-Shift (counter AI last)
                    return counter_to(ai_last);
                } else { // draw -> random switch
                    int m = rng.sample_move_uniform();
                    if (m==pl_last) m = (m+1)%3;
                    return m;
                }
            }
            case 4: { // cyclic
                if (t==0) return cycleCur;
                if (cycleDir>0) { // R->S->P->R (0->1->2->0)
                    cycleCur = (cycleCur + 1) % 3;
                } else { // R->P->S->R (0->2->1->0)
                    cycleCur = (cycleCur + 2) % 3;
                }
                return cycleCur;
            }
            case 5: { // Markov
                if (t==0) return markovCur;
                double r = rng.randu(), acc=0;
                for(int j=0;j<3;++j){ acc+=markov[markovCur][j]; if(r<=acc){ markovCur=j; break; } }
                return markovCur;
            }
            case 6: { // copier
                if (t==0) return rng.sample_move_uniform();
                return ai_last;
            }
            case 7: { // anti-copier
                if (t==0) return rng.sample_move_uniform();
                return counter_to(ai_last);
            }
            case 8: { // streaky: repeat with p, reset after loss
                if (t==0) return streakCur;
                if (res_last==1) { // player lost -> reset baseline
                    streakCur = rng.sample_move_uniform();
                }
                if (rng.randu()<p_repeat) return streakCur;
                // flip to non-baseline
                int m = rng.sample_move_uniform();
                if (m==streakCur) m = (m+1)%3;
                streakCur = m;
                return m;
            }
            case 9: { // tilt: after 2 losses -> tiltMove with high prob
                if (t==0) return rng.sample_move_uniform();
                if (loseStreak>=2 && rng.randu()<tiltBias) return tiltMove;
                // otherwise random-ish, slightly biased away from ai_last
                int m = rng.sample_move_uniform();
                if (m==ai_last && rng.randu()<0.6) m = (m+1)%3;
                return m;
            }
            case 10: { // gambler: avoid repeats
                if (t==0) return rng.sample_move_uniform();
                double q_repeat = repeat_penalty; // small probability to repeat
                double r = rng.randu();
                if (r < q_repeat) return pl_last;
                // choose among other two
                int a = (pl_last+1)%3, b = (pl_last+2)%3;
                return (rng.randu()<0.5? a:b);
            }
            case 11: { // tie-reactive
                if (t==0) { last_tie_move=-1; return rng.sample_move_uniform(); }
                if (res_last==0) {
                    last_tie_move = pl_last;
                    // after tie: switch to counter of the tied move
                    return counter_to(pl_last);
                } else {
                    // otherwise, mild tendency to keep
                    if (rng.randu()<0.6) return pl_last;
                    int m = rng.sample_move_uniform();
                    if (m==pl_last) m=(m+1)%3;
                    return m;
                }
            }
        }
        return rng.sample_move_uniform();
    }

    static string type_name(int t){
        switch(t){
            case 0: return "Uniform";
            case 1: return "Biased";
            case 2: return "WSLS";
            case 3: return "LSWS";
            case 4: return "Cyclic";
            case 5: return "Markov";
            case 6: return "Copier";
            case 7: return "Anti-Copier";
            case 8: return "Streaky";
            case 9: return "Tilt";
            case 10:return "Gambler";
            case 11:return "Tie-reactive";
        }
        return "Unknown";
    }
};

// ============================================================
// Features builder (for RNN per-step input)
// Inputs per time step x_t from history up to t-1:
//  - one-hot last player's move (3)        -> last_pl_oh
//  - one-hot last AI move (3)              -> last_ai_oh
//  - one-hot last outcome (3: W/D/L)       -> last_res_oh (AI perspective)
//  - step index t/10                       -> 1 scalar
//  - score diff so far (wins - losses)/10  -> 1 scalar
//  - frequencies of player's moves so far  -> 3 scalars
//  - length of current player's streak/10  -> 1 scalar
// Total D = 3 + 3 + 3 + 1 + 1 + 3 + 1 = 15
// ============================================================

struct FeatureBuilder {
    static const int D = 15;

    static void build_for_step(const EpisodeHistory& H, int t, vector<double>& x) {
        x.assign(D, 0.0);
        int idx = 0;
        int n = (int)H.pl.size();
        // history up to t-1: but H contains steps < t at inference time
        // We pass H truncated externally to t steps. Here, just use H.
        // last player's move one-hot:
        if (n>0) x[idx + H.pl.back()] = 1.0;
        idx += 3;
        // last AI move one-hot:
        if (n>0) x[idx + H.ai.back()] = 1.0;
        idx += 3;
        // last outcome one-hot (W/D/L) from AI view:
        if (n>0) {
            int r = H.res.back();
            int oh = (r==1?0:(r==0?1:2));
            x[idx + oh] = 1.0;
        }
        idx += 3;
        // step fraction
        x[idx++] = (double)t / 10.0;
        // score diff normalized
        int score=0; for(int v: H.res) score += v;
        x[idx++] = (double)score / 10.0;
        // player move frequencies so far
        double cnt[3]={0,0,0};
        for(int m: H.pl) cnt[m]+=1.0;
        double denom = max(1, (int)H.pl.size());
        for(int j=0;j<3;++j) x[idx++] = cnt[j]/denom;
        // streak length of player's repeats
        int streak = 0;
        if (!H.pl.empty()) {
            int last = H.pl.back();
            for(int i=(int)H.pl.size()-1;i>=0;--i){
                if (H.pl[i]==last) streak++;
                else break;
            }
        }
        x[idx++] = (double)streak/10.0;
    }
};

// ============================================================
// Simple RNN (Elman) with two heads (move prediction 3-way, archetype 12-way)
// h_t = tanh(Wxh x_t + Whh h_{t-1} + bh)
// y_t = softmax(Wy h_t + by)        (player next move distribution)
// c_t = softmax(Wc h_t + bc)        (archetype posterior)
// Loss = sum_t CE(y_t, y_true_t) + alpha * sum_t w_t * CE(c_t, class)
// SGD+momentum, L2, grad clipping
// ============================================================

struct RNN {
    int D, H, O, C; // input, hidden, output(move=3), classes(=12)
    // params
    vector<double> Wxh, Whh, bh;
    vector<double> Wy, by;
    vector<double> Wc, bc;
    // momentum
    vector<double> vWxh, vWhh, vbh, vWy, vby, vWc, vbc;

    // hyperparams
    double lr = 0.03;
    double momentum = 0.9;
    double weight_decay = 1e-5;
    double clip = 5.0;
    double alpha_cls = 0.25; // weight for class head

    RNN(int D_, int H_, int O_=3, int C_=12): D(D_), H(H_), O(O_), C(C_) {
        Wxh.assign(H*D, 0); Whh.assign(H*H,0); bh.assign(H,0);
        Wy.assign(O*H, 0); by.assign(O,0);
        Wc.assign(C*H, 0); bc.assign(C,0);
        vWxh.assign(H*D, 0); vWhh.assign(H*H,0); vbh.assign(H,0);
        vWy.assign(O*H, 0); vby.assign(O,0);
        vWc.assign(C*H, 0); vbc.assign(C,0);
    }

    void xavier_init(RNG& rng){
        auto urand = [&](double r){ return (rng.randu()*2.0-1.0)*r; };
        {
            double r = sqrt(6.0/(D+H));
            for(double& w: Wxh) w = urand(r);
        }
        {
            double r = sqrt(6.0/(H+H));
            for(double& w: Whh) w = urand(r);
        }
        {
            double r = sqrt(6.0/(H+O));
            for(double& w: Wy) w = urand(r);
        }
        {
            double r = sqrt(6.0/(H+C));
            for(double& w: Wc) w = urand(r);
        }
        // biases zero
    }

    static void softmax(const vector<double>& z, vector<double>& p){
        p.resize(z.size());
        double m = *max_element(z.begin(), z.end());
        double s = 0.0;
        for(size_t i=0;i<z.size();++i){ p[i] = exp(z[i]-m); s+=p[i]; }
        for(size_t i=0;i<z.size();++i){ p[i] /= s>0? s:1.0; }
    }

    struct ForwardCache {
        vector<vector<double>> x;      // L x D
        vector<vector<double>> h;      // L x H
        vector<vector<double>> yprob;  // L x O
        vector<vector<double>> cprob;  // L x C
        int L=0;
    };

    ForwardCache forward_seq(const vector<vector<double>>& X) const {
        int L = (int)X.size();
        ForwardCache cache;
        cache.L=L;
        cache.x = X;
        cache.h.assign(L, vector<double>(H,0.0));
        cache.yprob.assign(L, vector<double>(O,0.0));
        cache.cprob.assign(L, vector<double>(C,0.0));
        vector<double> hprev(H, 0.0);

        for(int t=0;t<L;++t){
            const vector<double>& xt = X[t];
            vector<double>& ht = cache.h[t];
            // ht_pre = Wxh*xt + Whh*hprev + bh
            for(int i=0;i<H;++i){
                double s = bh[i];
                // Wxh
                const double* w1 = &Wxh[i*D];
                for(int j=0;j<D;++j) s += w1[j]*xt[j];
                // Whh
                const double* w2 = &Whh[i*H];
                for(int j=0;j<H;++j) s += w2[j]*hprev[j];
                ht[i] = tanh(s);
            }
            // y_t
            vector<double> ylogit(O, 0.0);
            for(int i=0;i<O;++i){
                double s = by[i];
                const double* wy = &Wy[i*H];
                for(int j=0;j<H;++j) s += wy[j]*ht[j];
                ylogit[i] = s;
            }
            softmax(ylogit, cache.yprob[t]);
            // class head
            vector<double> clogit(C,0.0);
            for(int i=0;i<C;++i){
                double s = bc[i];
                const double* wc = &Wc[i*H];
                for(int j=0;j<H;++j) s += wc[j]*ht[j];
                clogit[i]=s;
            }
            softmax(clogit, cache.cprob[t]);
            hprev = ht;
        }
        return cache;
    }

    struct Grad {
        vector<double> dWxh, dWhh, dbh, dWy, dby, dWc, dbc;
        double loss_move=0.0, loss_cls=0.0;
    };

    // labels_y: size L, each in [0..2]
    // class_label: in [0..C-1] or -1 to disable class loss
    Grad backward_seq(const ForwardCache& cache,
                      const vector<int>& labels_y,
                      int class_label) const {
        int L = cache.L;
        Grad g;
        g.dWxh.assign(H*D,0.0); g.dWhh.assign(H*H,0.0); g.dbh.assign(H,0.0);
        g.dWy.assign(O*H,0.0); g.dby.assign(O,0.0);
        g.dWc.assign(C*H,0.0); g.dbc.assign(C,0.0);
        vector<double> dh_next(H, 0.0);
        const double eps = 1e-12;

        for(int t=L-1;t>=0;--t){
            const vector<double>& ht = cache.h[t];
            const vector<double>& y = cache.yprob[t];
            const vector<double>& c = cache.cprob[t];

            // action head
            vector<double> dy(O,0.0);
            for(int i=0;i<O;++i) dy[i] = y[i];
            int ytrue = labels_y[t];
            if (ytrue>=0 && ytrue<O) {
                dy[ytrue] -= 1.0;
                g.loss_move += -log(max(y[ytrue], eps));
            }
            // dWy, dby and contribution to dh
            vector<double> dh(H,0.0);
            for(int i=0;i<O;++i){
                for(int j=0;j<H;++j){
                    g.dWy[i*H + j] += dy[i]*ht[j];
                    dh[j] += Wy[i*H + j]*dy[i];
                }
                g.dby[i] += dy[i];
            }

            // class head (weighted by schedule)
            if (class_label>=0 && class_label<C) {
                double w = (double)(t+1)/ (double)L;
                vector<double> dc(C,0.0);
                for(int i=0;i<C;++i) dc[i] = c[i];
                dc[class_label] -= 1.0;
                // scale by alpha * w
                for(int i=0;i<C;++i) dc[i] *= (alpha_cls * w);
                g.loss_cls += (alpha_cls*w) * (-log(max(c[class_label], eps)));

                for(int i=0;i<C;++i){
                    for(int j=0;j<H;++j){
                        g.dWc[i*H + j] += dc[i]*ht[j];
                        dh[j] += Wc[i*H + j]*dc[i];
                    }
                    g.dbc[i] += dc[i];
                }
            }

            // back through tanh
            vector<double> dtanh(H,0.0);
            for(int j=0;j<H;++j){
                double grad_h = dh[j] + dh_next[j];
                dtanh[j] = (1.0 - ht[j]*ht[j]) * grad_h;
            }

            // dWxh, dWhh, dbh
            const vector<double>& xt = cache.x[t];
            for(int i=0;i<H;++i){
                g.dbh[i] += dtanh[i];
                // Wxh row i
                for(int j=0;j<D;++j) g.dWxh[i*D + j] += dtanh[i]*xt[j];
                // Whh row i
                const vector<double>& hprev = (t>0 ? cache.h[t-1] : vector<double>(H,0.0));
                for(int j=0;j<H;++j) g.dWhh[i*H + j] += dtanh[i]*(t>0 ? hprev[j] : 0.0);
            }
            // dh_next = Whh^T * dtanh
            fill(dh_next.begin(), dh_next.end(), 0.0);
            for(int j=0;j<H;++j){
                double s=0.0;
                for(int i=0;i<H;++i) s += Whh[i*H + j]*dtanh[i];
                dh_next[j] = s;
            }
        }
        return g;
    }

    void apply_weight_decay(Grad& g) {
        // L2 only on weights (not biases)
        for(size_t i=0;i<Wxh.size();++i) g.dWxh[i] += weight_decay * Wxh[i];
        for(size_t i=0;i<Whh.size();++i) g.dWhh[i] += weight_decay * Whh[i];
        for(size_t i=0;i<Wy.size(); ++i) g.dWy[i]  += weight_decay * Wy[i];
        for(size_t i=0;i<Wc.size(); ++i) g.dWc[i]  += weight_decay * Wc[i];
    }

    void clip_gradients(Grad& g) const {
        double norm2=0.0;
        auto acc = [&](const vector<double>& v){ for(double x:v) norm2+=x*x; };
        acc(g.dWxh); acc(g.dWhh); acc(g.dbh);
        acc(g.dWy); acc(g.dby); acc(g.dWc); acc(g.dbc);
        double n = sqrt(norm2);
        if (n > clip) {
            double s = clip / n;
            auto scale = [&](vector<double>& v){ for(double& x: v) x*=s; };
            scale(g.dWxh); scale(g.dWhh); scale(g.dbh);
            scale(g.dWy);  scale(g.dby);  scale(g.dWc); scale(g.dbc);
        }
    }

    void sgd_update(const Grad& g) {
        auto upd = [&](vector<double>& W, vector<double>& V, const vector<double>& G){
            for(size_t i=0;i<W.size();++i){
                V[i] = momentum * V[i] + G[i];
                W[i] -= lr * V[i];
            }
        };
        upd(Wxh, vWxh, g.dWxh);
        upd(Whh, vWhh, g.dWhh);
        upd(bh,  vbh,  g.dbh);
        upd(Wy,  vWy,  g.dWy);
        upd(by,  vby,  g.dby);
        upd(Wc,  vWc,  g.dWc);
        upd(bc,  vbc,  g.dbc);
    }

    // Train on one episode sequence {X, y}, with optional class label
    pair<double,double> train_episode(const vector<vector<double>>& X,
                          const vector<int>& labels_y, int class_label) {
        auto cache = forward_seq(X);
        auto g = backward_seq(cache, labels_y, class_label);
        apply_weight_decay(g);
        clip_gradients(g);
        sgd_update(g);
        return {g.loss_move / max(1, cache.L), g.loss_cls}; // avg move CE per step, cls loss (weighted sum over steps)
    }

    // Predict y_t and class posterior given history up to t (X_prefix)
    void predict_step(const vector<vector<double>>& X, vector<double>& p_move, vector<double>& p_cls) const {
        auto cache = forward_seq(X);
        if (cache.L==0) { p_move = {1.0/3,1.0/3,1.0/3}; p_cls = vector<double>(C, 1.0/C); return; }
        p_move = cache.yprob.back();
        p_cls  = cache.cprob.back();
    }
};

// ============================================================
// Data generation and training / evaluation
// ============================================================

struct CLI {
    int trainN = 20000;
    int valN = 0;
    int testN = 0;
    bool play = false;
    uint32_t seed = 42u;
    bool log = false;
    bool debug_reveal = false;
    double eps = 0.05;
};

static void print_help(){
    cout << "Usage: ./rps_nn [--train N] [--val N] [--test N] [--play] [--seed S] [--log] [--debug-reveal] [--eps E]\n";
    cout << "Episode length is fixed to 10 rounds.\n";
}

static CLI parse_cli(int argc, char** argv){
    CLI c;
    for(int i=1;i<argc;++i){
        string a=argv[i];
        if (a=="--train" && i+1<argc) c.trainN = stoi(argv[++i]);
        else if (a=="--val" && i+1<argc) c.valN = stoi(argv[++i]);
        else if (a=="--test" && i+1<argc) c.testN = stoi(argv[++i]);
        else if (a=="--play") c.play = true;
        else if (a=="--seed" && i+1<argc) c.seed = (uint32_t)stoul(argv[++i]);
        else if (a=="--log") c.log = true;
        else if (a=="--debug-reveal") c.debug_reveal = true;
        else if (a=="--eps" && i+1<argc) c.eps = stod(argv[++i]);
        else if (a=="--help" || a=="-h") { print_help(); exit(0); }
        else { /* ignore unknown */ }
    }
    return c;
}

struct Episode {
    EpisodeHistory H;
    vector<vector<double>> X; // per-step features
    vector<int> y;            // true player moves at each step
    int class_label;
};

// Generate episode for TRAINING (AI moves are random to excite opponent behaviors)
static Episode generate_episode_for_training(RNG& rng, int type){
    Opponent opp(type, rng);
    Episode ep;
    ep.class_label = type;
    ep.H = EpisodeHistory{};
    const int L=10;
    for(int t=0;t<L;++t){
        int ai_move = rng.sample_move_uniform(); // random AI move for data generation
        int pl_move = opp.next_move(ep.H, rng);
        int r = result_ai(ai_move, pl_move);
        ep.H.ai.push_back(ai_move);
        ep.H.pl.push_back(pl_move);
        ep.H.res.push_back(r);
    }
    // build features for each step t using prefix up to t-1
    ep.X.resize(L);
    ep.y.resize(L);
    EpisodeHistory prefix;
    for(int t=0;t<L;++t){
        FeatureBuilder::build_for_step(prefix, t, ep.X[t]);
        ep.y[t] = ep.H.pl[t];
        // advance prefix with the actually played moves of step t
        prefix.ai.push_back(ep.H.ai[t]);
        prefix.pl.push_back(ep.H.pl[t]);
        prefix.res.push_back(ep.H.res[t]);
    }
    return ep;
}

// Evaluate policy using the model on-policy against opponents
struct Stats {
    struct ByType {
        long rounds=0, correct=0;
        long w=0,d=0,l=0;
        long episodes=0;
        long cls_correct=0;
    };
    vector<ByType> byType;
    Stats(){ byType.assign(12, ByType()); }
};

static int argmax(const vector<double>& v){
    return (int)(max_element(v.begin(), v.end()) - v.begin());
}

static int policy_ai_from_pred(const vector<double>& p_move, RNG& rng, double eps){
    // ε-greedy: with prob eps pick random, else counter to argmax
    if (rng.randu() < eps) return rng.sample_move_uniform();
    int predicted = argmax(p_move);
    return counter_to(predicted);
}

static Stats evaluate_model(RNN& net, RNG& rng, int N, bool log=false, double eps=0.05){
    Stats S;
    const int L=10;
    for(int e=0;e<N;++e){
        int type = e % 12; // balanced across archetypes
        Opponent opp(type, rng);
        EpisodeHistory hist;
        vector<int> y_true;
        vector<vector<double>> Xprefix;
        Xprefix.reserve(L);

        int cls_true = type;
        for(int t=0;t<L;++t){
            // build features for current step from history
            vector<double> xt;
            FeatureBuilder::build_for_step(hist, t, xt);
            // forward
            Xprefix.push_back(xt);
            vector<double> p_move, p_cls;
            net.predict_step(Xprefix, p_move, p_cls);
            int ai_move = policy_ai_from_pred(p_move, rng, eps);

            if (log && t==0) {
                cout << "[Eval] Ep#" << e+1 << " Type="<<Opponent::type_name(type)<<"\n";
            }
            if (log) {
                cout << "  t="<<t+1<<"/10  P(player)=[";
                cout.setf(std::ios::fixed); cout<<setprecision(2)<<p_move[0]<<","<<p_move[1]<<","<<p_move[2]<<"]";
                cout << "  AI="<<move_to_char(ai_move)<<" ";
            }

            // Opponent move (reacting to history incl. AI last)
            int pl_move = opp.next_move(hist, rng);

            int r = result_ai(ai_move, pl_move);
            hist.ai.push_back(ai_move);
            hist.pl.push_back(pl_move);
            hist.res.push_back(r);

            int pred_move = argmax(p_move);
            S.byType[type].rounds++;
            if (pred_move == pl_move) S.byType[type].correct++;
            if (r==1) S.byType[type].w++; else if (r==0) S.byType[type].d++; else S.byType[type].l++;
            if (log) {
                cout << " PL="<<move_to_char(pl_move)<<" res="<<(r==1?"W":(r==0?"D":"L")) << "\n";
            }

            if (t==L-1){
                // final class estimate
                vector<double> tmp1,tmp2;
                net.predict_step(Xprefix, tmp1, tmp2);
                int cls_pred = argmax(tmp2);
                S.byType[type].episodes++;
                if (cls_pred==cls_true) S.byType[type].cls_correct++;
            }
        }
    }
    return S;
}

// Train loop
static void train_offline(RNN& net, RNG& rng, int N, bool log){
    const int types = 12;
    double avg_loss=0, avg_cls=0;
    for(int e=0;e<N;++e){
        int type = e % types;
        auto ep = generate_episode_for_training(rng, type);
        auto losses = net.train_episode(ep.X, ep.y, ep.class_label);
        avg_loss += losses.first;
        avg_cls  += losses.second;
        if (log && (e+1)%1000==0){
            cout << "[Train] Ep "<<(e+1)<<"/"<<N
                 <<"  MoveCE="<<setprecision(4)<<fixed<< (avg_loss/1000.0)
                 <<"  ClsLoss="<< (avg_cls/1000.0) << "\n";
            avg_loss=0; avg_cls=0;
        }
        // simple lr schedule
        if ((e+1)%5000==0) net.lr *= 0.7;
    }
}

// ============================================================
// Interactive play (10 rounds) with online fine-tuning
// ============================================================

static void online_update_prefix(RNN& net, const EpisodeHistory& hist, int steps=1){
    // train on the prefix observed so far (sequence length = len(hist))
    int L = (int)hist.pl.size();
    if (L==0) return;
    vector<vector<double>> X(L);
    vector<int> y(L);
    EpisodeHistory prefix;
    for(int t=0;t<L;++t){
        FeatureBuilder::build_for_step(prefix, t, X[t]);
        y[t] = hist.pl[t];
        // advance
        if ((int)prefix.pl.size() < L) {
            prefix.ai.push_back(hist.ai[t]);
            prefix.pl.push_back(hist.pl[t]);
            prefix.res.push_back(hist.res[t]);
        }
    }
    double old_lr = net.lr;
    net.lr = max(0.01, old_lr * 0.7); // slightly smaller step for online
    for(int s=0;s<steps;++s) net.train_episode(X, y, -1);
    net.lr = old_lr;
}

static void play_interactive(RNN& net, RNG& rng, bool log, bool debug_reveal, double eps){
    cout << "Игра: Камень (K/k), Ножницы (N/n), Бумага (B/b). 10 раундов.\n";
    EpisodeHistory hist;
    vector<vector<double>> Xprefix;
    int scoreW=0, scoreD=0, scoreL=0;
    int correct=0;
    for(int t=0;t<10;++t){
        vector<double> xt;
        FeatureBuilder::build_for_step(hist, t, xt);
        Xprefix.push_back(xt);

        vector<double> p_move, p_cls;
        net.predict_step(Xprefix, p_move, p_cls);
        int ai_move = policy_ai_from_pred(p_move, rng, eps);

        cout << "\nРаунд "<<t+1<<"/10\n";
        // cout << "Прогноз игрока P=[K:"<<fixed<<setprecision(2)<<p_move[0]
        //      <<", N:"<<p_move[1]<<", B:"<<p_move[2]<<"]";
        // if (debug_reveal) cout << "  [AI заранее: "<<move_to_char(ai_move)<<"]";
        cout << "\nВаш ход (k/n/b): ";
        string s;
        int pl_move=-1;
        while(true){
            if (!(cin>>s)) { cout << "\nEOF. Выход.\n"; return; }
            pl_move = parse_move(s);
            if (pl_move>=0 && pl_move<3) break;
            cout << "Неверный ввод. Введите k/n/b: ";
        }

        cout << "ИИ сыграл: " << move_to_char(ai_move) << "  Вы: " << move_to_char(pl_move) << "  -> ";
        int r = result_ai(ai_move, pl_move);
        if (r==1){ cout << "Победа ИИ\n"; scoreW++; }
        else if (r==0){ cout << "Ничья\n"; scoreD++; }
        else { cout << "Поражение ИИ\n"; scoreL++; }

        if (argmax(p_move)==pl_move) correct++;

        hist.ai.push_back(ai_move);
        hist.pl.push_back(pl_move);
        hist.res.push_back(r);

        // онлайн-додообучение (на всей пройденной префиксной последовательности)
        online_update_prefix(net, hist, 2);

        // классификация архетипа (топ-3)
        net.predict_step(Xprefix, p_move, p_cls);
        vector<pair<double,int>> tmp;
        for(int i=0;i<12;++i) tmp.push_back({p_cls[i], i});
        sort(tmp.begin(), tmp.end(), greater<>());
        cout << "Топ-3 архетипа: ";
        for(int k=0;k<3;++k){
            int id = tmp[k].second;
            cout << Opponent::type_name(id) << " ("<<fixed<<setprecision(2)<<tmp[k].first<<")";
            if (k<2) cout << ", ";
        }
        cout << "\nСчет: W/D/L = " << scoreW << "/" << scoreD << "/" << scoreL << "\n";
    }
    int total=scoreW+scoreD+scoreL;
    double wr = (double)scoreW / max(1,total);
    double acc = (double)correct / max(1,total);
    cout << "\nИтог: W/D/L = "<<scoreW<<"/"<<scoreD<<"/"<<scoreL
         <<"  |  Винрейт ИИ="<<fixed<<setprecision(3)<<wr
         <<"  |  Точность предсказаний="<<acc<<"\n";
}

// ============================================================
// Main
// ============================================================

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    // cin.tie(nullptr);

    CLI cli = parse_cli(argc, argv);
    RNG rng(cli.seed);

    const int D = FeatureBuilder::D;
    const int H = 64;
    const int O = 3;
    const int C = 12;

    RNN net(D, H, O, C);
    net.xavier_init(rng);
    net.lr = 0.03;
    net.momentum = 0.9;
    net.weight_decay = 1e-5;
    net.clip = 5.0;
    net.alpha_cls = 0.25;

    if (cli.trainN > 0) {
        cout << "Офлайн-тренировка: "<<cli.trainN<<" эпизодов (seed="<<cli.seed<<")\n";
        train_offline(net, rng, cli.trainN, cli.log);
    }

    if (cli.valN > 0) {
        cout << "\nВалидация на синтетике: "<<cli.valN<<" эпизодов\n";
        auto S = evaluate_model(net, rng, cli.valN, cli.log, cli.eps);
        long R=0, Ccorr=0, W=0,Dd=0,Ll=0, Eps=0, ClsCorr=0;
        cout << "Архетипы (точность предсказаний, винрейт ИИ, классификация):\n";
        cout << fixed << setprecision(3);
        for(int t=0;t<12;++t){
            auto &b=S.byType[t];
            R += b.rounds; Ccorr += b.correct; W+=b.w; Dd+=b.d; Ll+=b.l; Eps+=b.episodes; ClsCorr+=b.cls_correct;
            double acc = (double)b.correct / max(1L,b.rounds);
            double wr  = (double)b.w / max(1L,(b.w+b.d+b.l));
            double cl  = (double)b.cls_correct / max(1L,b.episodes);
            cout << " - " << setw(13) << left << Opponent::type_name(t)
                 << " Acc="<<acc<<"  WR="<<wr<<"  Cls="<<cl<<"\n";
        }
        cout << "ИТОГО: Acc="<<((double)Ccorr/max(1L,R))
             <<"  WR="<<((double)W/max(1L,(W+Dd+Ll)))
             <<"  Cls="<<((double)ClsCorr/max(1L,Eps))<<"\n";
    }

    if (cli.testN > 0) {
        cout << "\nТест на синтетике: "<<cli.testN<<" эпизодов\n";
        auto S = evaluate_model(net, rng, cli.testN, cli.log, cli.eps);
        long R=0, Ccorr=0, W=0,Dd=0,Ll=0, Eps=0, ClsCorr=0;
        cout << fixed << setprecision(3);
        for(int t=0;t<12;++t){
            auto &b=S.byType[t];
            R += b.rounds; Ccorr += b.correct; W+=b.w; Dd+=b.d; Ll+=b.l; Eps+=b.episodes; ClsCorr+=b.cls_correct;
        }
        cout << "TOTAL: Acc="<<((double)Ccorr/max(1L,R))
             <<"  WR="<<((double)W/max(1L,(W+Dd+Ll)))
             <<"  Cls="<<((double)ClsCorr/max(1L,Eps))<<"\n";
    }

    if (cli.play) {
        // quick warmup if user didn't pretrain
        if (cli.trainN==0) {
            cout << "Быстрый прогрев на синтетике (10000 эп.)...\n";
            train_offline(net, rng, 10000, false);
        }
        cout << "\nИнтерактивная игра:\n";
        play_interactive(net, rng, cli.log, cli.debug_reveal, cli.eps);
    }

    if (!cli.play && cli.trainN==0 && cli.valN==0 && cli.testN==0) {
        print_help();
    }
    return 0;
}