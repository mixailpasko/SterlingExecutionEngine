#include "OrderMgr.h"
#import "C:\\Windows\SysWOW64\Sti\Sterling.tlb" no_namespace    /*Location of Sterling Lib*/
#include "External.h"

/*Log4cplus Files*/
#include <log4cplus/fileappender.h> 
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <iomanip>
#include "ExecEngine.h"

/* Boost UUID Generation */
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp> 
#include <boost/uuid/uuid_io.hpp>    
#include <boost\spirit\include\karma.hpp>

#include "RTypes.pb.h"
#include "MySQL.h"
#include "UpdateTrade.h"
#include "Utility.h"

namespace Framework
{
	namespace ExecutionEngine
	{
		OrderManager::OrderManager()
		{
			db.InitiateDBConnection();
		}

		void OrderManager::SendDemoSterlingOrder()
		{	
			Serializable::CustROrder_ser rorder;
			rorder.set_bstraccount("DEMO888");
			rorder.set_side("B");
			rorder.set_bstrsymbol("MSFT"); //Does not fill
			rorder.set_nquantity(12);
			float dprice = 0.020010000;
			rorder.set_lmtprice(dprice);
			rorder.set_pricetype(CustROrder_ser_EPriceType_Lmt);
			rorder.set_tif("D");
			rorder.set_bstrdestination("NYSE");
			rorder.set_bstrclorderid(boost::uuids::to_string(boost::uuids::random_generator()()).c_str());
			rorder.set_cust_ordercategory("O");
		
			ISTIOrderPtr com_cust_ord(__uuidof(STIOrder));
			com_cust_ord->Account = _com_util::ConvertStringToBSTR(rorder.bstraccount().c_str());
			com_cust_ord->Side = _com_util::ConvertStringToBSTR(rorder.side().c_str());;
			com_cust_ord->Symbol = _com_util::ConvertStringToBSTR(rorder.bstrsymbol().c_str());
			com_cust_ord->Quantity = rorder.nquantity();
			STIPriceTypes pt = rorder.pricetype() == CustROrder_ser::EPriceType::CustROrder_ser_EPriceType_Lmt ? STIPriceTypes::ptSTILmt : STIPriceTypes::ptSTIMkt;
			com_cust_ord->PriceType = pt;
			com_cust_ord->Tif = _com_util::ConvertStringToBSTR(rorder.tif().c_str());
			com_cust_ord->Destination = _com_util::ConvertStringToBSTR(rorder.bstrdestination().c_str());
			com_cust_ord->ClOrderID = _com_util::ConvertStringToBSTR(rorder.bstrclorderid().c_str());

			//-------------
			com_cust_ord->OpenClose =  _com_util::ConvertStringToBSTR(rorder.cust_ordercategory().c_str());
			com_cust_ord->Maturity =   _com_util::ConvertStringToBSTR(rorder.maturity().c_str());//rorder.maturity();
			com_cust_ord->PutCall =    _com_util::ConvertStringToBSTR(rorder.putcall().c_str());
			com_cust_ord->Instrument = _com_util::ConvertStringToBSTR(rorder.instrument().c_str());
			com_cust_ord->StrikePrice = rorder.strikeprice();

			float f = 2.34;
			double d = QString::number(f,'g',4).toDouble();
			//com_cust_ord->LmtPrice = f; //2.39999991
			double dpr = QString::number(rorder.lmtprice(),'g',2).toDouble();
			double testa =  0.02;
			com_cust_ord->LmtPrice = dpr;//dpr; //2.4 // With DOUBLE

			//User field is in base 64
			com_cust_ord->User =      _com_util::ConvertStringToBSTR(rorder.bstruser().c_str());//_com_util::ConvertStringToBSTR(rorder.formid().c_str());  //Notice the form id is being attached to the user field
			com_cust_ord->Underlying = _com_util::ConvertStringToBSTR(rorder.underlying().c_str());
			//-------------
		
			int a = com_cust_ord->SubmitOrder();
            //ProcessOrder(a,rorder);

		}

		//This method sends out the options
		void OrderManager::SendOptionsOrder(Serializable::CustROrder_ser& rorder)
		{
			Serializable::ExtraRFields extra;
			//Convert it back to binary
			std::string serializedString = Framework::Utility::CommnUtility().base64_decode(rorder.bstruser()); 
			extra.ParseFromString(serializedString); 
			if(extra.ordertype()=="N")
			{
				ISTIOrderPtr com_cust_ord(__uuidof(STIOrder));
				com_cust_ord->Account = _com_util::ConvertStringToBSTR(rorder.bstraccount().c_str());
				com_cust_ord->Side = _com_util::ConvertStringToBSTR(rorder.side().c_str());;
				com_cust_ord->Symbol = _com_util::ConvertStringToBSTR(rorder.bstrsymbol().c_str());
				com_cust_ord->Quantity = rorder.nquantity();
				STIPriceTypes pt = rorder.pricetype() == CustROrder_ser::EPriceType::CustROrder_ser_EPriceType_Lmt ? STIPriceTypes::ptSTILmt : STIPriceTypes::ptSTIMkt;
				com_cust_ord->PriceType = pt;
				com_cust_ord->Tif = _com_util::ConvertStringToBSTR(rorder.tif().c_str());
				com_cust_ord->Destination = _com_util::ConvertStringToBSTR(rorder.bstrdestination().c_str());
				com_cust_ord->ClOrderID = _com_util::ConvertStringToBSTR(rorder.bstrclorderid().c_str());

				//-------------
				com_cust_ord->OpenClose =  _com_util::ConvertStringToBSTR(rorder.cust_ordercategory().c_str());
				com_cust_ord->Maturity =   _com_util::ConvertStringToBSTR(rorder.maturity().c_str());//rorder.maturity();
				com_cust_ord->PutCall =    _com_util::ConvertStringToBSTR(rorder.putcall().c_str());
				com_cust_ord->Instrument = _com_util::ConvertStringToBSTR(rorder.instrument().c_str());
				com_cust_ord->StrikePrice = rorder.strikeprice();
							
				//com_cust_ord->LmtPrice = rorder.lmtprice();
				com_cust_ord->LmtPrice = QString::number(rorder.lmtprice(),'g',2).toDouble(); //Resolves the issue of 2.33999999, will appear as 2.34
				
				//User field is in base 64
				com_cust_ord->User =      _com_util::ConvertStringToBSTR(rorder.bstruser().c_str());//_com_util::ConvertStringToBSTR(rorder.formid().c_str());  //Notice the form id is being attached to the user field
				com_cust_ord->Underlying = _com_util::ConvertStringToBSTR(rorder.underlying().c_str());
				//-------------

				int a = com_cust_ord->SubmitOrder();
				ProcessOrder(a,rorder);
			}
			else
			{
				 ISTIOrderMaintPtr com_canc_ord(__uuidof(STIOrderMaint));
				 BSTR user_account = _com_util::ConvertStringToBSTR(rorder.bstraccount().c_str());
                 BSTR Order_to_cancel_id =  _com_util::ConvertStringToBSTR(rorder.cust_cancelorderid().c_str());
				 BSTR new_id = _com_util::ConvertStringToBSTR(rorder.bstrclorderid().c_str());
				 com_canc_ord->CancelOptionsOrder(user_account,0,Order_to_cancel_id,new_id);
			}
			
		}//end method


		void OrderManager::ConfirmOrder(Serializable::CustROrder_ser& rorder)
		{
			db.UpdateOrder(rorder);
		}

		void OrderManager::ProcessOrder(int ret_code,Serializable::CustROrder_ser& rorder)
		{
			switch (ret_code)
			{
			case 0:    
				{
					//Add To Order Records
					LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Order Received by sterling - Response Code: " + QString::number(ret_code).toStdString()); 
					rorder.set_nlvsquantity(rorder.nquantity());
					rorder.set_norderstatus(-99);
					rorder.set_cust_cancelorderid("-99");
					//--------------------------------------------------------------------------------
					Serializable::ExtraRFields extra;
					
					//Convert it back to binary
					std::string serializedString = Framework::Utility::CommnUtility().base64_decode(rorder.bstruser()); 
					
					extra.ParseFromString(serializedString); 
                    //--------------------------------------------------------------------------------
					db.InsertNewOptionOrder(rorder,extra.formid());
					break;
				}
			case -1:  //The account used is not valid/permissioned for the login
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "The account used is not valid/permissioned for the login");  break; }
			case -2:  //Not a valid side 
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Not a valid Side");   break; }
			case -3:  //Invalid Qty 
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Invalid Quantity");   break; }
			case -4:  //Invalid Symbol
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Invalid Symbol"); 	   break; }
			case -5:  //Invalid PriceType
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Invalid PriceType");  break; }			 
			case -6:  //Invalid Tif
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Invalid Tif");        break; }
			case -7:  //Invalid Destination
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Invalid Destination"); break;}
			case -8:  //Exposure Limit Violation
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Exposure Limit Violation"); break; }
			case -9:  //NYSE+ Rules Violation
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "NYSE+ Rules Violation");    break; }
			case -10:  //NYSE+ 30-Second Violation
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "NYSE+ 30 Seconds Rule Violation"); break; }
			case -11:  //Disable SelectNet Short Sales
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Disable SelectNet Short Sales");   break; }
			case -12:  //Long Sale Position Rules Violation
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Long Sale Position Rules Violation"); break; }
			case -13:  //Short Sale Position Rules Violation
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Short Sale Position Rules Violations");break;}			 
			default:
				{ LOG4CPLUS_INFO(ExecEngine::EngineLogger, "Undocumented Error Code : " + ret_code ); break; }
			}//End Switch
		}

		//Insert the new trade update in the DB
		void OrderManager::ProcessTradeUpdate(Framework::ExecutionEngine::TradeUpdate &Tupdate)
		{
			db.InsertNewTrade(Tupdate);
		}

		Serializable::CustROrder_ser OrderManager::RetrieveOrderByID(std::string& id)
		{
			return db.ReturnOrderByID(id);
		}

		Serializable::CustROrder_ser OrderManager::RetrieveOrderByRecordID(const std::string& id)
		{
			return db.ReturnOrderByRecordID(id);
		}
	}//end namespace StratManager
}//end namespace Framework