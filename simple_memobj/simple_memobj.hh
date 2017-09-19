#ifndef __LEARNING_GEM5_SIMPLE_MEMOBJ_SIMPLE_MEMOBJ_HH__
#define __LEARNING_GEM5_SIMPLE_MEMOBJ_SIMPLE_MEMOBJ_HH__

#include <vector>

#include "mem/mem_object.hh"
#include "params/SimpleMemobj.hh"

class SimpleMemobj : public MemObject{
	private:

		/////////////////////////INNER CLASSES///////////////////////

		class CPUSidePort : public SlavePort{

			private:
				SimpleMemobj *owner;
				bool needRetry;	
				PacketPtr blockedPacket;

			public:
				CPUSidePort(const std::string& name, SimpleMemobj *owner):
					SlavePort(name, owner), owner(owner)
				{}

				AddrRangeList getAddrRanges() const override;
				
				void sendPacket(PacketPtr pkt);
				void trySendRetry();

			protected:
				Tick recvAtomic(PacketPtr pkt) override {panic("recvAtomic unimpl.");}
				void recvFunctional(PacketPtr pkt) override;
				bool recvTimingReq(PacketPtr pkt) override;
				void recvRespRetry() override;
		};

		class MemSidePort : public MasterPort{
			
			private:
				SimpleMemobj *owner;
				PacketPtr blockedPacket;

			public:
				MemSidePort(const std::string& name, SimpleMemobj *owner):
					MasterPort(name, owner), owner(owner) 
				{}

				void sendPacket(PacketPtr pkt);

			protected:
				bool recvTimingResp(PacketPtr pkt) override;
				void recvReqRetry() override;
				void recvRangeChange() override;
		};
		
		AddrRangeList getAddrRanges() const;

		void handleFunctional(PacketPtr pkt);
		
		void sendRangeChange();
		
		bool handleRequest(PacketPtr pkt);
		
		bool handleResponse(PacketPtr pkt);

		////////////////////////////PORTS DECLARATION//////////////////////

		CPUSidePort instPort;
		CPUSidePort dataPort;
		MemSidePort memPort;
	
		bool blocked;

	public:
		
		//Constructor
		SimpleMemobj(SimpleMemobjParams *params);
		

		//override virtual ftns of MemObject
		BaseMasterPort& getMasterPort(const std::string& if_name,
				PortID idx = InvalidPortID) override;

		BaseSlavePort& getSlavePort(const std::string& if_name,
				PortID idx = InvalidPortID) override;

};
#endif
