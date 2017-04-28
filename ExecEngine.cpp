#include "ExecEngine.h"
#include "SterlingEvnt.h"
#include "External.h"
#include "MySQL.h"
#include "boost/algorithm/string.hpp"

//Boost UUID Generator
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp> 
#include <boost/uuid/uuid_io.hpp>    

/*Log4cplus Files does not work with UNICODE Preprocessor*/
#include <log4cplus/fileappender.h> 
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <iomanip>

#include <boost/thread/thread.hpp>
#include "Network.h"
#include "RTypes.pb.h"
#include "OrderMgr.h"
#include "External.h"

#import "C:\\Windows\SysWOW64\Sti\Sterling.tlb" no_namespace    /*Location of Sterling Lib*/

#define PROTOBUF_USE_DLLS

using namespace Serializable;
using namespace log4cplus;

log4cplus::SharedAppenderPtr ExecEngine::EngineAppender;
Framework::ExecutionEngine::DBaseConnection ExecEngine::db;

std::auto_ptr<log4cplus::Layout> ExecEngine::EngineLayout;
log4cplus::Logger ExecEngine::EngineLogger;

bool ExecEngine::ConfigInitialized=false;;

ExecEngine::ExecEngine(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
	ui.setupUi(this);
	QObject::connect(ui.cmdStartEngine, SIGNAL(clicked()), this, SLOT(StartExecutionEngine()));
	QObject::connect(ui.actionClear_Trades,SIGNAL(triggered()), this, SLOT(ClearTradesTable()));
	QObject::connect(ui.actionClear_Orders,SIGNAL(triggered()), this, SLOT(ClearOrdersTable()));
	
	//Initialize SQL Server Connectivity
	db.InitiateDBConnection();
}


void ExecEngine::ClearOrdersTable()
{
	db.ClearOrdersTable();
}


void ExecEngine::ClearTradesTable()
{
	db.ClearTradesTable();
}

ExecEngine::~ExecEngine()
{/*Destrcutor*/}


void ExecEngine::InitializeLoggingLibrary()
{	
	EngineAppender = new log4cplus::FileAppender(LoggingFile);
	EngineAppender->setName("ExecutionAppender");
	EngineLayout = std::auto_ptr<log4cplus::Layout>(new log4cplus::TTCCLayout());;
	EngineAppender->setLayout( EngineLayout );
	EngineLogger = log4cplus::Logger::getInstance("EngLogger");
	EngineLogger.addAppender(EngineAppender);
	EngineLogger.setLogLevel ( log4cplus::INFO_LOG_LEVEL ); 
}

void ExecEngine::InitializeConfigSettings()
{
	if(ConfigInitialized)
	{return;}

	std::vector<std::string> results;
	std::ifstream File(FilePath);
	std::string line;
	if(File)
	{
		while(getline(File,line))
		{
			if(line!="")
			{
				boost::split(results, line, boost::is_any_of("=//"));
				std::string key_ = results[0];
				std::string value_ = results[1];
				boost::algorithm::trim(key_);
				boost::algorithm::trim(value_);
				get_config_map().insert(std::pair<std::string,std::string>(key_,value_));
			}
		}
		File.close();
		ConfigInitialized = true;
	}
}//End method


std::map<std::string,std::string>& ExecEngine::get_config_map()
{
	static std::map<std::string,std::string> map;
	return map;
}

std::string ExecEngine::GetConfigValue(std::string item_)
{
	InitializeConfigSettings();
	std::map<std::string,std::string>::iterator myiter = get_config_map().find(item_);
	if(myiter != get_config_map().end())
	{
		//item found
		return get_config_map()[item_];
	}
	else
	{
		__debugbreak();
		return "Item not found";
	}
}

void ExecEngine::SubscribeSterlingEvents(HRESULT hr)
{
	//Register Events
	ISTIAppPtr app(__uuidof(STIApp));
	ISTIEventsPtr evnt(__uuidof(STIEvents));

	_bstr_t nme = app->GetTraderName();
	app->SetModeXML(true);
	bool a = app->IsApiEnabled();
	evnt->SetOrderEventsAsStructs(true);

	IConnectionPointContainer *point_container = NULL;
	IConnectionPoint *conn_point = NULL;

	//See if this implements a sink type interface
	//retrieve from a pointer to an interface that the object implements
	hr = evnt->QueryInterface(IID_IConnectionPointContainer,(void**)&point_container);

	//Get Interface Pointer to the Events
	hr = point_container->FindConnectionPoint(__uuidof(_ISTIEventsEvents),&conn_point);

	DWORD cookie;
	Framework::ExecutionEngine::SterlingEventSink *snk = new Framework::ExecutionEngine::SterlingEventSink();

	//Pass custom Interface to the Events
	hr = conn_point->Advise(snk,&cookie);

	if(hr!=S_OK)
	{
		__debugbreak();
	}

}//End method


void ExecEngine::SendDemoOrder()
{
  Framework::ExecutionEngine::OrderManager().SendDemoSterlingOrder();
}

void ExecEngine::StartExecutionEngine()
{
	
	bool bRet;
	MSG msg;
	
	//S_FALSE: The COM library is already initialized on main
	//HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );

	//Initialize Logging Library
	InitializeLoggingLibrary();

	//Subscribe to Sterling Events
	SubscribeSterlingEvents(hr);
	

	//Start the OrderReceiver and The component Responder
	Framework::ExecutionEngine::QpidConnector qc;
	qc.InitiateUpdatePackSender();     //For sending out update EVENTS
	boost::thread t(&Framework::ExecutionEngine::QpidConnector::StartOptionOrderReceiver,qc); //For Receiving orders
	

	ui.cmdStartEngine->setText("STOP Execution Engine");
	
	//Send Test message
	//SendDemoOrder();

	//Start Processing Events
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}//End method