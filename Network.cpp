#include "Network.h"
#include <intrin.h>
#include <iostream>

#include <iostream> 
#include <sstream> 
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "External.h"
#include "SterlingEvnt.h"
#include "ExecEngine.h"

//Boost UUID Generator
#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp> 
#include <boost/uuid/uuid_io.hpp>   

//QPID Header Files
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Receiver.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>


//Custom Option Types 
#include "RTypes.pb.h"

#import "C:\\Windows\SysWOW64\Sti\Sterling.tlb" no_namespace    /*Location of Sterling Lib*/

using namespace qpid::messaging;

namespace Framework
{
	namespace ExecutionEngine
	{

		//########################################################################
		//This method receives orders from the Strategy Manager                  #
		//This should run in a parallel thread                                   #
		//THIS THREAD HAS TO INITIALIZE COM SINCE IT USES STI TYPE LIB           #
		//########################################################################
		void QpidConnector::StartOptionOrderReceiver()
		{
			//Initialize the COM Componenet for this thread
			//HRESULT hr = CoInitialize(NULL);
			HRESULT hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
						
			Message msg;
			std::string QpidAddress = ExecEngine::GetConfigValue("QpidOrderEventAddress");
			Connection connection(QpidAddress);
			connection.open();
			Session session  = connection.createSession();
			//Receiver receiver = session.createReceiver("Option_Orders_; {create:always, delete:always}");
			Receiver receiver = session.createReceiver("Option_Orders_; {create:always}");
			receiver.setCapacity(800);
			int msg_counter = 0;
			while(true)
			{
				try
				{
					msg = receiver.fetch(Duration::FOREVER);
					Serializable::CustROrder_ser cust;
					cust.ParseFromString(msg.getContent());
					omgr.SendOptionsOrder(cust);
					msg_counter++;
					if(msg_counter>600)
					{
						session.acknowledge();
						msg_counter = 0;
					}
				}
				catch (std::exception &ex)
				{
					std::string err = ex.what();
					__debugbreak();
				}
			}//End while
			
		}//end Method



		//#############################################################################
		//This method sends out EVENTS                                                #
		//This method initiates the update packet dispatcher sender object            #
		//#############################################################################
        void QpidConnector::InitiateUpdatePackSender()
		{
			try
			{
				std::string queue_name = "Update_packet";
				std::string sender_string = queue_name +";{create:always}";						
				qpid::messaging::Connection connection(ExecEngine::GetConfigValue("QpidOrderEventAddress"));
				connection.open();
				connection.setOption("tcp-nodelay", true);
				Session session  = connection.createSession();
				UpdatePackSender = session.createSender(sender_string) ;
				UpdatePackSender.setCapacity(800);
			}
			catch(std::exception& ex)
			{
				std::string e = ex.what();
			}
		}

		//Dispatch the EVENT Packet
		void QpidConnector::DispatchUpdatePacket(Serializable::RUpdatePacket& opt)
		{
			try
			{
				UpdatePackSender.send(opt.SerializeAsString());
			}
			catch(std::exception& ex)
			{
				std::string e = ex.what();
			}
		}
	}//end namespace ExecutionEngine
}//end namespace Framework