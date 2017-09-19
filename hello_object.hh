#ifndef __LEARNING_GEMS_HELLO_OBJECT_HH__
#define __LEARNING_GEMS_HELLO_OBJECT_HH__

#include "params/HelloObject.hh"
#include "sim/sim_object.hh"

class HelloObject : public SimObject{
	
	private:
		void processEvent();
		void processEvent2();

		EventWrapper<HelloObject, &HelloObject::processEvent> event;
		EventWrapper<HelloObject, &HelloObject::processEvent2> event2;

		GoodbyeObject& goodbye;

		std::string myName;
		Tick latency;
		int timesLeft;
		int timesLeft2;

	public:
		HelloObject(HelloObjectParams *p);

		void startup();
};

#endif
