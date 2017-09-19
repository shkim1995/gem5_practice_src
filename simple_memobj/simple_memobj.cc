#include "learning_gem5/simple_memobj/simple_memobj.hh"

#include "debug/SimpleMemobj.hh"

SimpleMemobj::SimpleMemobj(SimpleMemobjParams *params):
	MemObject(params),
	instPort(params->name+".inst_port", this),
	dataPort(params->name+".data_port", this),
	memPort(params->name+".mem_side", this)
{}
//////////////////////FTN for PORT ASSIGNMENT/////////////////////

BaseMasterPort&
SimpleMemobj::getMasterPort(const std::string& if_name, PortID idx){
	if(if_name == "mem_side"){
		return memPort;
	} else{
		return MemObject::getMasterPort(if_name, idx);
	}
}

BaseSlavePort&
SimpleMemobj:: getSlavePort(const std::string& if_name, PortID idx){
	if(if_name == "inst_port") return instPort;
	else if(if_name == "data_port") return dataPort;
	else return MemObject::getSlavePort(if_name, idx);
}

////////////////////ADDR RANGE FTNS & FUNCTIONAL FTNS////////////////////

AddrRangeList
SimpleMemobj::CPUSidePort::getAddrRanges() const{
	return owner->getAddrRanges();
}

void
SimpleMemobj::CPUSidePort::recvFunctional(PacketPtr pkt){
	return owner->handleFunctional(pkt);
}

void
SimpleMemobj::handleFunctional(PacketPtr pkt){
	memPort.sendFunctional(pkt);
}

AddrRangeList
SimpleMemobj::getAddrRanges() const{
	DPRINTF(SimpleMemobj, "Sending new ranges\n");
	return memPort.getAddrRanges();
}

void 
SimpleMemobj::MemSidePort::recvRangeChange(){
	owner->sendRangeChange();
}

void 
SimpleMemobj::sendRangeChange(){
	instPort.sendRangeChange();
	dataPort.sendRangeChange();
}

///////////////////////RECIEVING REQUEST FTNS//////////////////////

bool
SimpleMemobj::CPUSidePort::recvTimingReq(PacketPtr pkt){
	if(!owner->handleRequest(pkt)){
		needRetry = true;
		return false;
	} else{
		return true;
	}
}

bool
SimpleMemobj::handleRequest(PacketPtr pkt){
	if(blocked) return false;

	DPRINTF(SimpleMemobj, "Got request for addr %#x\n", pkt->getAddr());

	//blocked until the mem object got response from the mem-side port for this request
	//than the packet will be reserved in the pair port of CPU-side port
	blocked = true;
	memPort.sendPacket(pkt);
	return true;
}

void 
SimpleMemobj::MemSidePort::sendPacket(PacketPtr pkt){
	panic_if(blockedPacket != nullptr, "Should Never Try to Sedn if Blocked!");
	if(!sendTimingReq(pkt)) blockedPacket = pkt;
}

void 
SimpleMemobj::MemSidePort::recvReqRetry(){
	assert(blockedPacket != nullptr);

	PacketPtr pkt = blockedPacket;
	blockedPacket = nullptr;

	sendPacket(pkt);
}

/////////////////////////RECIEVING RESPONSE FTNS/////////////////////

bool
SimpleMemobj::MemSidePort::recvTimingResp(PacketPtr pkt){
	return owner->handleResponse(pkt);
}

bool
SimpleMemobj::handleResponse(PacketPtr pkt){
	assert(blocked);
	DPRINTF(SimpleMemobj, "Got response for addr %#x\n", pkt->getAddr());

	blocked = false;

	if(pkt->req->isInstFetch()) instPort.sendPacket(pkt);
	else dataPort.sendPacket(pkt);

	instPort.trySendRetry();
	dataPort.trySendRetry();

	return true;
}

void 
SimpleMemobj::CPUSidePort::sendPacket(PacketPtr pkt){
	panic_if(blockedPacket != nullptr, "Should Never Try to Sendif Blocked!");
	if(!sendTimingResp(pkt)) blockedPacket = pkt;
}

void 
SimpleMemobj::CPUSidePort::recvRespRetry(){
	assert(blockedPacket != nullptr);

	PacketPtr pkt = blockedPacket;
	blockedPacket = nullptr;

	sendPacket(pkt);
}

void 
SimpleMemobj::CPUSidePort::trySendRetry(){
	if(needRetry && blockedPacket == nullptr){
		needRetry = false;
		DPRINTF(SimpleMemobj, "Sending retry req for %d\n", id);
		sendRetryReq();
	}
}

SimpleMemobj*
SimpleMemobjParams::create(){

    return new SimpleMemobj(this);
}
