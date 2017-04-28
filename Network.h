#ifndef Qpid_Network_Header
#define Qpid_Network_Header

#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include "External.h"
#include "OrderMgr.h"
#include "RTypes.pb.h"

using namespace qpid::messaging;

namespace Framework
{
	namespace ExecutionEngine 
	{
		class QpidConnector 
		{
		private:
        OrderManager omgr;
	    qpid::messaging::Sender UpdatePackSender;

		public:
		void StartOptionOrderReceiver();
		void InitiateUpdatePackSender();
		void DispatchUpdatePacket(Serializable::RUpdatePacket& opt);
		};

	}//end namespace ExecutionEngine
}//end namespace Framework
#endif 