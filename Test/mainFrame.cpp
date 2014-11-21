#include "mainFrame.h"
#include "../ung/Simulation.h"

const int mainFrame::ID_FILE_EXIT = ::wxNewId();
const int mainFrame::ID_FILE_RUN = ::wxNewId();
const int mainFrame::ID_HELP_ABOUT = ::wxNewId();

BEGIN_EVENT_TABLE( mainFrame, wxFrame )
	EVT_MENU( ID_FILE_EXIT, mainFrame::OnExit )
	EVT_MENU( ID_FILE_RUN, mainFrame::OnRun )
	EVT_MENU( ID_HELP_ABOUT, mainFrame::OnAbout )
END_EVENT_TABLE()


mainFrame::mainFrame( wxFrame *frame, const wxString& title )
	: wxFrame( frame, -1, title )
{
	wxMenuBar* mbar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu( wxT("") );
	fileMenu->Append( ID_FILE_EXIT, wxT("E&xit\tAlt+F4"), wxT("Exit the application") );
	fileMenu->Append( ID_FILE_RUN, wxT("Run\tCtrl+R"), wxT("Run Code") );
	mbar->Append( fileMenu, wxT("&File") );

	wxMenu* helpMenu = new wxMenu( wxT("") );
	helpMenu->Append( ID_HELP_ABOUT, wxT("&About\tF1"), wxT("Show info about this application") );
	mbar->Append( helpMenu, wxT("&Help") );
	
	SetMenuBar( mbar );
}

void mainFrame::OnExit( wxCommandEvent& event )
{
	Close();
}

#define TEST1
#define TEST2
#define TEST3
#define TEST4
//#define TEST5
#define TEST6

void mainFrame::OnRun( wxCommandEvent& event )
{
#ifdef TEST1
	{
		//Everything static -  just checking nothing accelerates
		//without being supposed to
		RectangularSimulationWrappedEdges sim(5, 5, nullptr, nullptr, nullptr, nullptr);
		for(size_t i=0; i<10; ++i)
			sim.Step(UNG_FLT(0.01));
		UNG_FLT horizontalSpeeds[25];
		UNG_FLT verticalSpeeds[25];
		sim.getHorizontalSpeeds(horizontalSpeeds);
		sim.getVerticalSpeeds(verticalSpeeds);
		for(size_t i=0; i<25; ++i)
			if(horizontalSpeeds[i]!=0)
				throw("Failed Test1 - horizontal speed check");
		for(size_t i=0; i<25; ++i)
			if(verticalSpeeds[i]!=0)
				throw("Failed Test1 - vertical speed check");
	}
#endif
#ifdef TEST2
	{
		//5x5 grid. Middle row moving right with speed of 2
		//m s-1, other cells moving right with speed of 1 m s-1
		//checking that horizontal advection is working correctly
		//in inviscid case
		const size_t nHoriz=5;
		const size_t nVert=5;
		const size_t nCells=nHoriz*nVert;
		UNG_FLT horizontalSpeeds[nCells];
		for(size_t i=0; i<nCells; ++i)
		{
			if(i>=nHoriz*2 && i<nHoriz*3)
				horizontalSpeeds[i]=UNG_FLT(2.0);
			else
				horizontalSpeeds[i]=UNG_FLT(1.0);
		}
		RectangularSimulationWrappedEdges sim(nHoriz, nVert, nullptr, nullptr, horizontalSpeeds, nullptr);
		for(size_t i=0; i<10; ++i)
			sim.Step(UNG_FLT(0.01));
		UNG_FLT verticalSpeeds[nCells];
		sim.getHorizontalSpeeds(horizontalSpeeds);
		sim.getVerticalSpeeds(verticalSpeeds);
		for(size_t i=0; i<25; ++i)
		{
			if(i>=nHoriz*2 && i<nHoriz*3)
			{
				if(horizontalSpeeds[i]!=2.0)
					throw("Failed Test2 - fast row horizontal speed check");
			}
			else if( horizontalSpeeds[i]!=1.0)
				throw("Failed Test2 - slow row horizontal speed check");
		for(size_t i=0; i<25; ++i)
			if(verticalSpeeds[i]!=0)
				throw("Failed Test2 - vertical speed check");
		}
	}
#endif
#ifdef TEST3
	{
		//5x5 grid. Middle colkumn moving up with speed of 2
		//m s-1, other cells moving up with speed of 1 m s-1
		//checking that horizontal advection is working correctly
		//in inviscid case
		const size_t nHoriz=5;
		const size_t nVert=5;
		const size_t nCells=nHoriz*nVert;
		UNG_FLT verticalSpeeds[nCells];
		size_t fastCol=nHoriz/2;
		for(size_t i=0; i<nCells; ++i)
		{
			if(i%nHoriz==fastCol)
				verticalSpeeds[i]=UNG_FLT(2.0);
			else
				verticalSpeeds[i]=UNG_FLT(1.0);
		}
		RectangularSimulationWrappedEdges sim(nHoriz, nVert, nullptr, nullptr, nullptr, verticalSpeeds);
		for(size_t i=0; i<10; ++i)
			sim.Step(UNG_FLT(0.01));
		UNG_FLT horizontalSpeeds[nCells];
		sim.getHorizontalSpeeds(horizontalSpeeds);
		sim.getVerticalSpeeds(verticalSpeeds);
		for(size_t i=0; i<25; ++i)
		{
			if(i%nHoriz==fastCol)
			{
				if(verticalSpeeds[i]!=2.0)
					throw("Failed Test3 - fast row vertical speed check");
			}
			else if( verticalSpeeds[i]!=1.0)
				throw("Failed Test3 - slow row vertical speed check");
		}
		for(size_t i=0; i<25; ++i)
			if(horizontalSpeeds[i]!=0)
				throw("Failed Test3 - horizontal speed check");
	}
#endif
#ifdef TEST4
	{
		//5x5 grid. Middle column moving up with speed of 0.1
		//m s-1, other cells stationary, but with solid boundaries
		const size_t nHoriz=5;
		const size_t nVert=5;
		const size_t nCells=nHoriz*nVert;
		UNG_FLT verticalSpeeds[nCells];
		size_t fastCol=nHoriz/2;
		for(size_t i=0; i<nCells; ++i)
		{
			if(i%nHoriz==fastCol)
				verticalSpeeds[i]=UNG_FLT(0.1);
			else
				verticalSpeeds[i]=UNG_FLT(0.0);
		}
		RectangularSimulationSolidEdges sim(nHoriz, nVert, nullptr, nullptr, nullptr, verticalSpeeds);
		for(size_t i=0; i<1000; ++i)
			sim.Step(UNG_FLT(0.0001));
		UNG_FLT horizontalSpeeds[nCells];
		UNG_FLT densities[nCells];
		sim.getHorizontalSpeeds(horizontalSpeeds);
		sim.getVerticalSpeeds(verticalSpeeds);
		sim.getDensities(densities);
		std::fstream fout;
		fout.open("test4.csv", std::ios::out);
		fout << "density,horizontal speed,vertical speed\n";
		for(size_t i=0; i<nCells; ++i)
			fout << densities[i] << "," << horizontalSpeeds[i] << "," << verticalSpeeds[i] << "\n";
		fout.close();
		
	}
#endif
#ifdef TEST5
	{
		//pressure wave
		const size_t nHoriz=3;
		const size_t nVert=1;
		const size_t nCells=nHoriz*nVert;
		UNG_FLT densities[nCells];
		size_t peakWidth=20;
		for(size_t i=0; i<nCells; ++i)
		{
			if(i==nHoriz/2)
				densities[i]=1.0+1e-5;
			else
				densities[i]=1.0;
		} 

		RectangularSimulationWrappedEdges sim(nHoriz, nVert, densities, nullptr, nullptr, nullptr);
		for(size_t i=0; i<2000000; ++i)
		{
			sim.Step(UNG_FLT(0.00001));
		}
		UNG_FLT verticalSpeeds[nCells];
		UNG_FLT horizontalSpeeds[nCells];
		sim.getHorizontalSpeeds(horizontalSpeeds);
		sim.getVerticalSpeeds(verticalSpeeds);
		sim.getDensities(densities);
		std::fstream fout;
		fout.open("test5.csv", std::ios::out);
		fout << "density,horizontal speed,vertical speed\n";
		for(size_t i=0; i<nCells; ++i)
			fout << densities[i]-1.0 << "," << horizontalSpeeds[i] << "," << verticalSpeeds[i] << "\n";
		fout.close();
		/*for(size_t i=0; i<25; ++i)
		{
			if(i%nHoriz==fastCol)
			{
				if(verticalSpeeds[i]!=2.0)
					throw("Failed Test3 - fast row vertical speed check");
			}
			else if( verticalSpeeds[i]!=1.0)
				throw("Failed Test3 - slow row vertical speed check");
		}
		for(size_t i=0; i<25; ++i)
			if(horizontalSpeeds[i]!=0)
				throw("Failed Test3 - horizontal speed check");*/
	}
#endif
#ifdef TEST6
	{
		//pressure wave
		const size_t nHoriz=1000;
		const size_t nVert=1;
		const size_t nCells=nHoriz*nVert;
		UNG_FLT densities[nCells];
		size_t peakWidth=20;
		for(size_t i=0; i<nCells; ++i)
			densities[i]=1.0;
		for (size_t i=0; i<peakWidth*6; ++i)
			densities[i]*=1.0+sci::distribution::normal(double(i),double(peakWidth*3),double(peakWidth))*peakWidth*sqrt(2*M_PI)*1e-5; 

		std::fstream foutinit;
		foutinit.open("test5init.csv", std::ios::out);
		foutinit << "density\n";
		for(size_t i=0; i<nCells; ++i)
			foutinit << densities[i]-1.0 << "\n";
		foutinit.close();

		RectangularSimulationWrappedEdges sim(nHoriz, nVert, densities, nullptr, nullptr, nullptr);
		for(size_t i=0; i<2000000; ++i)
		{
			if(i%10000==0)
			{
				UNG_FLT verticalSpeeds[nCells];
				UNG_FLT horizontalSpeeds[nCells];
				sim.getHorizontalSpeeds(horizontalSpeeds);
				sim.getVerticalSpeeds(verticalSpeeds);
				sim.getDensities(densities);
				char filename[200];
				strcpy(filename,"test5_");
				itoa(i,filename+strlen(filename),10);
				strcat(filename,".csv");
				std::fstream fout;
				fout.open(filename, std::ios::out);
				fout << "density,horizontal speed,vertical speed\n";
				for(size_t i=0; i<nCells; ++i)
					fout << densities[i]-1.0 << "," << horizontalSpeeds[i] << "," << verticalSpeeds[i] << "\n";
				fout.close();
			}
			sim.Step(UNG_FLT(0.00001));
		}
		UNG_FLT verticalSpeeds[nCells];
		UNG_FLT horizontalSpeeds[nCells];
		sim.getHorizontalSpeeds(horizontalSpeeds);
		sim.getVerticalSpeeds(verticalSpeeds);
		sim.getDensities(densities);
		std::fstream fout;
		fout.open("test5.csv", std::ios::out);
		fout << "density,horizontal speed,vertical speed\n";
		for(size_t i=0; i<nCells; ++i)
			fout << densities[i]-1.0 << "," << horizontalSpeeds[i] << "," << verticalSpeeds[i] << "\n";
		fout.close();
		/*for(size_t i=0; i<25; ++i)
		{
			if(i%nHoriz==fastCol)
			{
				if(verticalSpeeds[i]!=2.0)
					throw("Failed Test3 - fast row vertical speed check");
			}
			else if( verticalSpeeds[i]!=1.0)
				throw("Failed Test3 - slow row vertical speed check");
		}
		for(size_t i=0; i<25; ++i)
			if(horizontalSpeeds[i]!=0)
				throw("Failed Test3 - horizontal speed check");*/
	}
#endif
	wxMessageBox("Done - Tests successful");
}

void mainFrame::OnAbout( wxCommandEvent& event )
{
	wxMessageBox( wxT("$projectname$ Version 1.00.0"), wxT("About cdpfog...") );
}

mainFrame::~mainFrame()
{
}