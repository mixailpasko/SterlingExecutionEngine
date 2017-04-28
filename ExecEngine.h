#ifndef EXECENGINE_H
#define EXECENGINE_H

#include <QtGui/QMainWindow>
#include "ui_ExecEngine.h"

/*Log4cplus Files*/
#include <log4cplus/fileappender.h> 
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <iomanip>
#include "MySQL.h"

using namespace log4cplus;

class ExecEngine : public QMainWindow
{
	Q_OBJECT

public:
	HRESULT hr;
	ExecEngine(QWidget *parent = 0, Qt::WFlags flags = 0);
	static log4cplus::Logger EngineLogger;
	static std::string GetConfigValue(std::string item_);
	static std::map<std::string,std::string>& get_config_map();
	static Framework::ExecutionEngine::DBaseConnection db;
	~ExecEngine();

private:
	static log4cplus::SharedAppenderPtr EngineAppender;
	static std::auto_ptr<log4cplus::Layout> EngineLayout;
	static bool ConfigInitialized;
	static void InitializeConfigSettings();
	Ui::ExecEngineClass ui;
	void InitializeLoggingLibrary();
	void SubscribeSterlingEvents(HRESULT hr);
	void SendDemoOrder();

public slots:
	void StartExecutionEngine();
	void ClearTradesTable();
	void ClearOrdersTable();
};

#endif // EXECENGINE_H
