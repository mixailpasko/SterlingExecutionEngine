#include "OptionHandler.h"
#include "UpdateTrade.h"
#include "Utility.h"

namespace Framework
{
	namespace ExecutionEngine
	{
		//Constructor
		OptionHandler::OptionHandler()
		{
			//Initiate the UpdatePacket queue in QPID
			qpid_connector.InitiateUpdatePackSender();
		}

		//Process an order confirm
		void OptionHandler::OrderConfirmEvent(Serializable::CustROrder_ser& obj)
		{
			//omgr.ConfirmOrder(obj); //Update the database with the new object
		}


		//Process an order rejected
		void OptionHandler::OrderRejectedEvent(Serializable::CustROrder_ser& obj)
		{

		}


		//Process an order update - this has the returned user field
		void OptionHandler::OrderUpdateEvent(Serializable::CustROrder_ser& obj)
		{
			if(obj.bstrclorderid()!="")
			{
				std::string lmessage = "EVENT : UPDATE OCCURED - Id : " + obj.bstrclorderid() + " CODE : " + QString::number(obj.norderstatus()).toStdString();
				LOG4CPLUS_INFO(ExecEngine::EngineLogger, lmessage); 

				omgr.ConfirmOrder(obj); //Update the database with the new object
				
				Serializable::CustROrder_ser* ser = new Serializable::CustROrder_ser(obj);
				Serializable::RUpdatePacket ru;
				ru.set_allocated_custord(ser);

				qpid_connector.DispatchUpdatePacket(ru);
			}
			
			//check if this was an order that was canceled by the user
			else if(obj.norderstatus()==1 && obj.bstrclorderid()=="")
			{
				//Retrieve the actual object
				std::string order_record_id = obj.norderrecordid();
				Serializable::CustROrder_ser sstackorder = omgr.RetrieveOrderByRecordID(order_record_id);
				Serializable::CustROrder_ser* ser = new Serializable::CustROrder_ser(sstackorder);
				ser->set_norderstatus(8);
				Serializable::RUpdatePacket ru;
				ru.set_allocated_custord(ser);
				qpid_connector.DispatchUpdatePacket(ru); 
			}
		}//end method


		//Process a trade update for options
		void OptionHandler::TradeUpdateEvent(Framework::ExecutionEngine::TradeUpdate& tu)
		{
			if(tu.get_client_id()!="")
			{
				omgr.ProcessTradeUpdate(tu);
				
				//-----------------------------------------------------------------------------------
				//--------WE NEED TO SEND THR TRADE UPDATE TOO - INORDER TO UPDATE POSITIONS---------
				//-----------------------------------------------------------------------------------
				//This is for updating positions in OptionQuoter
				Serializable::CustROrder_ser sstackorder = omgr.RetrieveOrderByID(tu.get_client_id());

				Serializable::CustROrder_ser* ser = new Serializable::CustROrder_ser(sstackorder);
				Serializable::TradeUpdate* ser_tu = new Serializable::TradeUpdate();
				CopyTradeUpdt(*ser_tu,tu);
				Serializable::RUpdatePacket ru;
				ru.set_allocated_custord(ser);
				ru.set_allocated_tupdate(ser_tu);
				qpid_connector.DispatchUpdatePacket(ru);
				//-----------------------------------------------------------------------------------
				//-----------------------------------------------------------------------------------
			}
		}


		void OptionHandler::CopyTradeUpdt(Serializable::TradeUpdate& ser_tu,Framework::ExecutionEngine::TradeUpdate& tu)
		{
			ser_tu.set_symbol(tu.get_symbol());
            ser_tu.set_side(tu.get_side())  ;
			ser_tu.set_volume(tu.get_volume());
			ser_tu.set_exec_price(tu.get_exec_price());
			ser_tu.set_client_id(tu.get_client_id());
			ser_tu.set_datetimetime(tu.get_time());
		}

	}//End namespace ExecutionEngine

}//End namespace framework