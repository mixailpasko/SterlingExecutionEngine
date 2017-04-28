#ifndef MySQL_Header
#define MySQL_Header

#include <iostream>
#include <string>

#include <boost/optional.hpp>

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
#include "UpdateTrade.h"

using namespace Serializable;
namespace Framework
{
	namespace ExecutionEngine
	{
		class DBaseConnection
		{
			private:
				sql::Driver *driver;
				sql::Statement *stmt;
				sql::ResultSet *res;
				sql::Connection *con;
				sql::PreparedStatement *prep_stmt;

			public:
				void InitiateDBConnection();
				void InsertNewOptionOrder(Serializable::CustROrder_ser& otype,const std::string FormGUID );
				void UpdateOrder(Serializable::CustROrder_ser& otype);
				void InsertNewTrade(Framework::ExecutionEngine::TradeUpdate& tupdate);
				std::string ExtractFormNameById(std::string& client_id );
				Serializable::CustROrder_ser ReturnOrderByID(const std::string Order_ID);
				Serializable::CustROrder_ser ReturnOrderByRecordID(const std::string Order_ID);
				void ClearTradesTable();
				void ClearOrdersTable();
		};

	}//end namespace ExecutionEngine
}//end namespace Framework
#endif 