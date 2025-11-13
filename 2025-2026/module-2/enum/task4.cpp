#include <iostream>
#include <string>


enum class Event {
  LOGIN=0,
  LOGOUT=1,
  MESSAGESENT=2,
  FILEUPLOADED=3,
  ERROROCCURRED=4,
  PAYMENTRECEIVED=5,
  USERREGISTERED=6,
};


struct SME {
  std::string user;
  Event event;
  int id;
};

Event event_handler(SME sme) {
  switch (sme.event) {
    case Event::LOGIN:
      return Event::LOGIN;
    case Event::LOGOUT:
      return Event::LOGOUT;
    case Event::MESSAGESENT:
      return Event::MESSAGESENT;
    case Event::FILEUPLOADED:
      return Event::FILEUPLOADED;
    case Event::ERROROCCURRED:
      return Event::ERROROCCURRED;
    case Event::PAYMENTRECEIVED:
      return Event::PAYMENTRECEIVED;
    case Event::USERREGISTERED:
      return Event::USERREGISTERED;
  }
}


std::string EventToString(Event event) {
  switch (event) {
    case Event::LOGIN:
      return "LOGIN";
    case Event::LOGOUT:
      return "LOGOUT";
    case Event::MESSAGESENT:
      return "MESSAGESENT";
    case Event::FILEUPLOADED:
      return "FILEUPLOADED";
    case Event::ERROROCCURRED:
      return "ERROROCCURRED";
    case Event::PAYMENTRECEIVED:
      return "PAYMENTRECEIVED";
    case Event::USERREGISTERED:
      return "USERREGISTERED";
  }
}



int main() {

  SME a1 {"Nike", Event::LOGIN, 1};
  SME a2 {"Nike", Event::LOGOUT, 2};
  SME a3 {"Nike", Event::LOGIN, 3};
  SME a4 {"Nike", Event::LOGOUT, 4};
  SME a5 {"Nike", Event::LOGIN, 5};


  std::cout << "a1: " << EventToString(event_handler(a1)) << "\n";
  std::cout << "a2: " << EventToString(event_handler(a2)) << "\n";
  std::cout << "a3: " << EventToString(event_handler(a3)) << "\n";
  std::cout << "a4: " << EventToString(event_handler(a4)) << "\n";
  std::cout << "a5: " << EventToString(event_handler(a5)) << "\n";


  return 42;
}