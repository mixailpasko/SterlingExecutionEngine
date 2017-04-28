#ifndef Sterling_OrderMgr_Header
#define Sterling_OrderMgr_Header

#include <iostream>
#include "RTypes.pb.h" 
#include "MySQL.h"
#include  "UpdateTrade.h"

namespace Framework
{
	namespace ExecutionEngine
	{
		class OrderManager
		{
		private:
			Framework::ExecutionEngine::DBaseConnection db;
		public:
			OrderManager();
			void SendDemoSterlingOrder();
			void SendOptionsOrder(Serializable::CustROrder_ser& rorder);
			void ProcessOrder(int ret_code,Serializable::CustROrder_ser& rorder);
			void ConfirmOrder(Serializable::CustROrder_ser& rorder);
			void ProcessTradeUpdate(Framework::ExecutionEngine::TradeUpdate &Tupdate);
			Serializable::CustROrder_ser RetrieveOrderByID(std::string& id);
			Serializable::CustROrder_ser RetrieveOrderByRecordID(const std::string& id);
		};
		
	}//end namespace ExecutionEngine
}//end namespace Framework
#endif