#include "learning_gem5/goodbye_object.hh"

#include "debug/Hello.hh"
#include "sim/sim_exit.hh"

GoodbyeObject::GoodbyeObject(GoodbyeObjectParams *param) :
	SimObject(param),
	event(*this),
	bandwidth(param->write_bandwidth),
	bufferSize(param->buffer_size),
	buffer(nullptr),
	bufferUsed(0){
	
	buffer = new char[bufferSize];
	DPRINTF(Hello, "Created the goodbye Object\n");
}

void 
GoodbyeObject::processEvent(){
	DPRINTF(Hello, "Processing the event!\n");
	fillBuffer();
}

void 
GoodbyeObject::sayGoodbye(std::string other_name){
	DPRINTF(Hello, "Saying goodbye to %s\n", other_name);

	message = "Goodbye "+other_name+"!! ";
	fillBuffer();
}

void 
GoodbyeObject::fillBuffer(){
	
	assert(message.length()>0);

	int bytes_copied = 0;

	for(auto it = message.begin();
			it<message.end() && bufferUsed < bufferSize-1;
			it++, bufferUsed++, bytes_copied++){
		buffer[bufferUsed] = *it;
	}

	if(bufferUsed < bufferSize-1){
		DPRINTF(Hello, "Scheduling another fillBuffer in %d ticks\n", bandwidth * bytes_copied);
		schedule(event, curTick() + bandwidth * bytes_copied);
	}

	else {
		DPRINTF(Hello, "Goodbye Done Copying!\n");

		exitSimLoop(buffer, 0, curTick() + bandwidth * bytes_copied);
	}
}

GoodbyeObject*
GoodbyeObjectParams::create(){
	return new GoodbyeObject(this);
}


	
