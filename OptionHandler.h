#ifndef Option_Handler_Header
#define Option_Handler_Header

#include <iostream>
#include "OrderMgr.h"
#include "RTypes.pb.h"
#include "UpdateTrade.h"
#include "Network.h"

namespace Framework
{
	namespace ExecutionEngine
	{
		class OptionHandler
		{
		private:
			Framework::ExecutionEngine::OrderManager omgr;
			Framework::ExecutionEngine::QpidConnector qpid_connector;
			void CopyTradeUpdt(Serializable::TradeUpdate& ser_tu,Framework::ExecutionEngine::TradeUpdate& tu);
		public:
			OptionHandler();
			void OrderConfirmEvent(Serializable::CustROrder_ser& obj);
			void OrderRejectedEvent(Serializable::CustROrder_ser& obj);
			void OrderUpdateEvent(Serializable::CustROrder_ser& obj);
			void TradeUpdateEvent(Framework::ExecutionEngine::TradeUpdate& tu);
		};

	}//end namespace ExecutionEngine
}//end namespace Framework
#endif 