
#include "SharedMemoryInProcessPhysicsC_API.h"
#include "../Utils/b3Clock.h"

#include "PhysicsClientSharedMemory.h"
#include"../ExampleBrowser/InProcessExampleBrowser.h"
#include <stdio.h>
#include <string.h>
#include "PhysicsServerExampleBullet2.h"

#include "../CommonInterfaces/CommonExampleInterface.h"
#include "InProcessMemory.h"

#include "Bullet3Common/b3Logging.h"
class InProcessPhysicsClientSharedMemoryMainThread : public PhysicsClientSharedMemory
{
    btInProcessExampleBrowserMainThreadInternalData* m_data;
   b3Clock m_clock;
 
public:
    
    InProcessPhysicsClientSharedMemoryMainThread(int argc, char* argv[], bool useInProcessMemory)
    {
        int newargc = argc+3;
        char** newargv = (char**)malloc(sizeof(void*)*newargc);
		char* t0 = (char*)"--unused";
        newargv[0] = t0;
		for (int i=0;i<argc;i++)
            newargv[i+1] = argv[i];
        newargv[argc+1]=(char*)"--logtostderr";
		newargv[argc+2]=(char*)"--start_demo_name=Physics Server";
        
        m_data = btCreateInProcessExampleBrowserMainThread(newargc,newargv, useInProcessMemory);
        SharedMemoryInterface* shMem = btGetSharedMemoryInterfaceMainThread(m_data);
        
        setSharedMemoryInterface(shMem);
    }
    
    virtual ~InProcessPhysicsClientSharedMemoryMainThread()
    {
        setSharedMemoryInterface(0);
        btShutDownExampleBrowserMainThread(m_data);
    }
    
    // return non-null if there is a status, nullptr otherwise
    virtual const struct SharedMemoryStatus* processServerStatus()
    {
		
		{
			if (btIsExampleBrowserMainThreadTerminated(m_data))
			{
				PhysicsClientSharedMemory::disconnectSharedMemory();
			}
		}
			{	
	   		unsigned long int ms = m_clock.getTimeMilliseconds();
			if (ms>2)
			{ 
				B3_PROFILE("m_clock.reset()");

				btUpdateInProcessExampleBrowserMainThread(m_data);
				m_clock.reset(); 
			}
		}
		{
			b3Clock::usleep(0);
		}
		const SharedMemoryStatus* stat = 0;

		{
			stat = PhysicsClientSharedMemory::processServerStatus();
		}

		return stat;
        
    }
    
    virtual bool submitClientCommand(const struct SharedMemoryCommand& command)
    {
//        btUpdateInProcessExampleBrowserMainThread(m_data);
        return PhysicsClientSharedMemory::submitClientCommand(command);
    }
    
};

B3_SHARED_API b3PhysicsClientHandle b3CreateInProcessPhysicsServerAndConnectMainThread(int argc, char* argv[])
{
    InProcessPhysicsClientSharedMemoryMainThread* cl = new InProcessPhysicsClientSharedMemoryMainThread(argc, argv, 1);
    cl->setSharedMemoryKey(SHARED_MEMORY_KEY+1);
    cl->connect();
    return (b3PhysicsClientHandle ) cl;
}

B3_SHARED_API	b3PhysicsClientHandle b3CreateInProcessPhysicsServerAndConnectMainThreadSharedMemory(int argc, char* argv[])
{
    InProcessPhysicsClientSharedMemoryMainThread* cl = new InProcessPhysicsClientSharedMemoryMainThread(argc, argv, 0);
    cl->setSharedMemoryKey(SHARED_MEMORY_KEY+1);
    cl->connect();
    return (b3PhysicsClientHandle ) cl;
}



class InProcessPhysicsClientSharedMemory : public PhysicsClientSharedMemory
{
	btInProcessExampleBrowserInternalData* m_data;
	char** m_newargv;

public:
	
	InProcessPhysicsClientSharedMemory(int argc, char* argv[], bool useInProcessMemory)
	{
		int newargc = argc+2;
		m_newargv = (char**)malloc(sizeof(void*)*newargc);
		char* t0 = (char*)"--unused";
		m_newargv[0] = t0;

		for (int i=0;i<argc;i++)
			m_newargv[i+1] = argv[i];
		
		char* t1 = (char*)"--start_demo_name=Physics Server";
		m_newargv[argc+1] = t1;
		m_data = btCreateInProcessExampleBrowser(newargc,m_newargv, useInProcessMemory);
		SharedMemoryInterface* shMem = btGetSharedMemoryInterface(m_data);
		setSharedMemoryInterface(shMem);
	}

	virtual ~InProcessPhysicsClientSharedMemory()
	{
		setSharedMemoryInterface(0);
		btShutDownExampleBrowser(m_data);
		free(m_newargv);
	}

};

B3_SHARED_API	b3PhysicsClientHandle b3CreateInProcessPhysicsServerAndConnect(int argc, char* argv[])
{	
	
	InProcessPhysicsClientSharedMemory* cl = new InProcessPhysicsClientSharedMemory(argc, argv, 1);
    cl->setSharedMemoryKey(SHARED_MEMORY_KEY+1);
    cl->connect();
	return (b3PhysicsClientHandle ) cl;
}
B3_SHARED_API	b3PhysicsClientHandle b3CreateInProcessPhysicsServerAndConnectSharedMemory(int argc, char* argv[])
{	
	
	InProcessPhysicsClientSharedMemory* cl = new InProcessPhysicsClientSharedMemory(argc, argv, 0);
    cl->setSharedMemoryKey(SHARED_MEMORY_KEY+1);
    cl->connect();
	return (b3PhysicsClientHandle ) cl;
}

class InProcessPhysicsClientExistingExampleBrowser : public PhysicsClientSharedMemory
{
	CommonExampleInterface*  m_physicsServerExample;
	SharedMemoryInterface* m_sharedMem;
	b3Clock m_clock;
	unsigned long long int m_prevTime;
public:
	InProcessPhysicsClientExistingExampleBrowser (struct GUIHelperInterface* guiHelper)
	{
		

		m_sharedMem = new InProcessMemory;
		CommonExampleOptions options(guiHelper);
		options.m_sharedMem = m_sharedMem;
			
		m_physicsServerExample = PhysicsServerCreateFuncBullet2(options);
		m_physicsServerExample ->initPhysics();
		m_physicsServerExample ->resetCamera();
		setSharedMemoryInterface(m_sharedMem);
		m_clock.reset();
		m_prevTime = m_clock.getTimeMicroseconds();

	}
	virtual ~InProcessPhysicsClientExistingExampleBrowser()
	{
		m_physicsServerExample->exitPhysics();
		//s_instancingRenderer->removeAllInstances();
		delete m_physicsServerExample;
		delete m_sharedMem;
	}

	 // return non-null if there is a status, nullptr otherwise
    virtual const struct SharedMemoryStatus* processServerStatus()
    {
		m_physicsServerExample->updateGraphics();

		unsigned long long int curTime = m_clock.getTimeMicroseconds();
		unsigned long long int dtMicro = curTime - m_prevTime;
		m_prevTime = curTime;

		double dt = double(dtMicro)/1000000.;

		m_physicsServerExample->stepSimulation(dt);
		{
			b3Clock::usleep(0);
		}
		const SharedMemoryStatus* stat = 0;

		{
			stat = PhysicsClientSharedMemory::processServerStatus();
		}

		return stat;
        
    }

	virtual void renderScene()
	{
		m_physicsServerExample->renderScene();
	}
	virtual void debugDraw(int debugDrawMode)
	{
		m_physicsServerExample->physicsDebugDraw(debugDrawMode);
	}
	virtual bool mouseMoveCallback(float x, float y)
	{
		return m_physicsServerExample->mouseMoveCallback(x,y);
	}
	virtual bool mouseButtonCallback(int button, int state, float x, float y)
	{
		return m_physicsServerExample->mouseButtonCallback(button,state,x,y);
	}
	
};

void b3InProcessDebugDrawInternal(b3PhysicsClientHandle clientHandle, int debugDrawMode)
{
	InProcessPhysicsClientExistingExampleBrowser* cl = (InProcessPhysicsClientExistingExampleBrowser*) clientHandle;
	cl->debugDraw(debugDrawMode);
}
void b3InProcessRenderSceneInternal(b3PhysicsClientHandle clientHandle)
{
	InProcessPhysicsClientExistingExampleBrowser* cl = (InProcessPhysicsClientExistingExampleBrowser*) clientHandle;
	cl->renderScene();
}

int b3InProcessMouseMoveCallback(b3PhysicsClientHandle clientHandle,float x,float y)
{
	InProcessPhysicsClientExistingExampleBrowser* cl = (InProcessPhysicsClientExistingExampleBrowser*) clientHandle;
	return cl->mouseMoveCallback(x,y);
}
int b3InProcessMouseButtonCallback(b3PhysicsClientHandle clientHandle, int button, int state, float x, float y)
{
	InProcessPhysicsClientExistingExampleBrowser* cl = (InProcessPhysicsClientExistingExampleBrowser*) clientHandle;
	return cl->mouseButtonCallback(button,state, x,y);
}


B3_SHARED_API	b3PhysicsClientHandle b3CreateInProcessPhysicsServerFromExistingExampleBrowserAndConnect(void* guiHelperPtr)
{
	GUIHelperInterface* guiHelper = (GUIHelperInterface*) guiHelperPtr;
	InProcessPhysicsClientExistingExampleBrowser* cl  = new InProcessPhysicsClientExistingExampleBrowser(guiHelper);
	//InProcessPhysicsClientFromGuiHelper* cl = new InProcessPhysicsClientFromGuiHelper(guiHelper);
    cl->connect();
	return (b3PhysicsClientHandle ) cl;
}
