#include "learning_gem5/hello_object.hh"
#include "learning_gem5/goodbye_object.hh"

#include "debug/Hello.hh"

HelloObject::HelloObject(HelloObjectParams *params) : 
	SimObject(params), 
	event(*this), 
	event2(*this), 
	goodbye(*params->goodbye_object),
	myName(params->name),
	latency(params->time_to_wait), 
	timesLeft(params->number_of_fires), 
	timesLeft2(params->number_of_fires) {
	DPRINTF(Hello, "created the hello object\n");
}

void
HelloObject::processEvent(){
	timesLeft--;
	DPRINTF(Hello, "Hello world! Processing the event!\n");
	if(timesLeft <= 0){ 
		DPRINTF(Hello, "Done event\n");
		goodbye.sayGoodbye(myName);
	}
	else schedule(event, curTick()+latency);
}

void
HelloObject::processEvent2(){
	timesLeft2--;
	DPRINTF(Hello, "Hello world! Processing the event2!\n");
	if(timesLeft2 <= 0) DPRINTF(Hello, "Done event2\n");
	else schedule(event2, curTick()+latency);
}

void 
HelloObject::startup(){
	schedule(event, latency);
	schedule(event2, latency/2);
}


HelloObject* 
HelloObjectParams::create(){
	return new HelloObject(this);
}

