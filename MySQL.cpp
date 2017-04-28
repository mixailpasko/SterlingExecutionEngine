#include "MySQL.h"
#include <iostream>
#include <intrin.h>  
#include <vector>
#include "ExecEngine.h"

//Boost UUID Generator
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp> 
#include <boost/uuid/uuid_io.hpp>    
#include <boost\spirit\include\karma.hpp>

/* MySQL Connector/C++ specific headers */
#include <driver.h>
#include <connection.h>
#include <statement.h>
#include <prepared_statement.h>
#include <resultset.h>
#include <metadata.h>
#include <resultset_metadata.h>
#include <exception.h>

#include "RTypes.pb.h"
#include "Utility.h"

using namespace Serializable;

typedef ExecEngine ExeEngineClass;


namespace Framework
{
	namespace ExecutionEngine
	{
		void DBaseConnection::InitiateDBConnection()
		{
			/* Create a connection */
			driver = get_driver_instance();	
			
			//Retrieve details from the config file
			con = 	driver->connect(ExeEngineClass::GetConfigValue("SQLServerName"), ExeEngineClass::GetConfigValue("SQLUserName"), ExeEngineClass::GetConfigValue("SQLPassword"));
			
			/* Connect to the MySQL database */
			con->setSchema("ExecutionEngine");
			stmt = con->createStatement();
		}


		void DBaseConnection::ClearOrdersTable()
		{
			std::string SQL = "TRUNCATE TABLE `ExecutionEngine`.`orders`;";
			stmt = con->createStatement();
			stmt->execute(SQL);
		}

		void DBaseConnection::ClearTradesTable()
		{
			std::string SQL = "TRUNCATE TABLE `ExecutionEngine`.`Trades`;";
			stmt = con->createStatement();
			stmt->execute(SQL);
		}

		std::string DBaseConnection::ExtractFormNameById(std::string& client_id )
		{
			std::string formid;
			std::string SQL ="SELECT `orders`.`FormGUID` FROM `ExecutionEngine`.`orders` where `bstrClOrderId` = ?";
			prep_stmt = con->prepareStatement(SQL);
			prep_stmt->setString(1,client_id);
			res = prep_stmt->executeQuery();
			if(res->next()) //If object exists
			{
				formid =  res->getString("FormGUID");
			}
			return formid;
		}

		void DBaseConnection::InsertNewOptionOrder(Serializable::CustROrder_ser& otype,const std::string FormGUID )
		{

			std::string SQL = "INSERT INTO `ExecutionEngine`.`orders`(`idStratOne`,`frequency`,`level`,`symbol`,`bstrClOrderId`,`side`,	`nOrderStatus`," 
				"`SerializedObject`,`OrderCategory`,`LvsQuantity`,`CancelOrderID`,`FormGUID`,`SterlingOrderRecordID` )VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)";


			//Database Dump
			prep_stmt = con->prepareStatement(SQL);
			prep_stmt->setString(1,boost::uuids::to_string(boost::uuids::random_generator()()));
			prep_stmt->setInt(2,0);  //Since OptionQuoter does not attach frequency or levels
			prep_stmt->setInt(3,0);  //Since OptionQuoter does not attach frequency or levels
			prep_stmt->setString(4,otype.bstrsymbol());
			prep_stmt->setString(5,otype.bstrclorderid());
			prep_stmt->setString(6,otype.side());
			prep_stmt->setInt(7,otype.norderstatus());
			
			//prep_stmt->setString(8,otype.SerializeAsString());
			std::string serialized = Framework::Utility::CommnUtility().base64_encode(reinterpret_cast<const unsigned char*>(otype.SerializeAsString().c_str()), otype.SerializeAsString().length());
			prep_stmt->setString(8,serialized);
			
			prep_stmt->setString(9,otype.cust_ordercategory()); //O (Open) or C (Close)
			prep_stmt->setInt(10,otype.nlvsquantity());
			prep_stmt->setString(11,otype.cust_cancelorderid());
			prep_stmt->setString(12,FormGUID);
			prep_stmt->setString(13,"NA");
			prep_stmt->execute();
			
		}

		//Currently used for
		//Confirming an order Event
		//Updating an order Event
		void DBaseConnection::UpdateOrder(Serializable::CustROrder_ser& otype)
		{
			//std::string SQL = "UPDATE `ExecutionEngine`.`orders` SET `SerializedObject` = ? , `nOrderStatus` = ? WHERE `bstrClOrderId` = ? ";
			std::string SQL = "UPDATE `ExecutionEngine`.`orders` SET `SerializedObject` = ? , `nOrderStatus` = ? , `SterlingOrderRecordID` = ?  WHERE `bstrClOrderId` = ? ";
			prep_stmt = con->prepareStatement(SQL);
			
			std::string serialized = otype.SerializeAsString();
			serialized = Framework::Utility::CommnUtility().base64_encode(reinterpret_cast<const unsigned char*>(serialized.c_str()), serialized.length());
			prep_stmt->setString(1,serialized);
			prep_stmt->setInt(2,otype.norderstatus());
			prep_stmt->setString(3,otype.norderrecordid());
			prep_stmt->setString(4,otype.bstrclorderid());
			prep_stmt->execute();	
			prep_stmt->close();
		}


		void DBaseConnection::InsertNewTrade(Framework::ExecutionEngine::TradeUpdate& tupdate)
		{
			std::string SQL ="INSERT INTO `ExecutionEngine`.`Trades`(`idTrades`,`Symbol`,`Volume`,`ExecPrice`,`Time`,`ClientOrderID`,`FormGUID`,`Side` )VALUES	( ?,?,?,?,?,?,?,?)";
			
			//First Extract the name of the form this symbol belongs to
			std::string form_name = ExtractFormNameById(tupdate.get_client_id());

			try
			{
				//Database Dump
				prep_stmt = con->prepareStatement(SQL);
				prep_stmt->setString(1,boost::uuids::to_string(boost::uuids::random_generator()()));
				prep_stmt->setString(2,tupdate.get_symbol());
				prep_stmt->setInt(3,tupdate.get_volume());
				prep_stmt->setDouble(4,tupdate.get_exec_price());
				prep_stmt->setString(5,tupdate.get_time());
				prep_stmt->setString(6,tupdate.get_client_id());
				prep_stmt->setString(7,form_name);
				prep_stmt->setString(8,tupdate.get_side());
				prep_stmt->execute();
			}
			catch(std::exception& e)
			{
				std::string err = e.what();
			}
			
		}
		

		//This method returns the serialized object by searching for the Order_ID
		Serializable::CustROrder_ser DBaseConnection::ReturnOrderByID(const std::string Order_ID)
		{
			Serializable::CustROrder_ser cord;
			std::string serialized_Object;
			std::string SQL ="SELECT `orders`.`SerializedObject` FROM `ExecutionEngine`.`orders` where `bstrClOrderId` = ?";
			prep_stmt = con->prepareStatement(SQL);
			prep_stmt->setString(1,Order_ID);
			res = prep_stmt->executeQuery();
			if(res->next()) //If object exists
			{
				serialized_Object =  res->getString("SerializedObject");
			}
			serialized_Object = Framework::Utility::CommnUtility().base64_decode(serialized_Object);
			cord.ParseFromString(serialized_Object);
			return cord;
		}



		//This method returns the serialized object by searching for the Order_ID
		Serializable::CustROrder_ser DBaseConnection::ReturnOrderByRecordID(const std::string Order_ID)
		{
			Serializable::CustROrder_ser cord;
			std::string serialized_Object;
			std::string SQL ="SELECT `orders`.`SerializedObject` FROM `ExecutionEngine`.`orders` where `SterlingOrderRecordID` = ?";
			prep_stmt = con->prepareStatement(SQL);
			prep_stmt->setString(1,Order_ID);
			res = prep_stmt->executeQuery();
			if(res->next()) //If object exists
			{
				serialized_Object =  res->getString("SerializedObject");
			}
			serialized_Object = Framework::Utility::CommnUtility().base64_decode(serialized_Object);
			cord.ParseFromString(serialized_Object);
			return cord;
		}

	}//End namespace
}//End namespace
