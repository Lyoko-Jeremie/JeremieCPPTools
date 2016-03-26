#ifndef TIMEGUARD_H
#define TIMEGUARD_H

// 必须在外面包含  好奇怪
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//#define TIME_CLOCK
#define CV_TICK
//#define CPP_CHRONO_HRC    // not test yet



#ifdef TIME_CLOCK
#include <ctime>
#endif // TIME_CLOCK

#ifdef CV_TICK
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif // CV_TICK


#ifdef CPP_CHRONO_HRC
#include <chrono>
#endif // CPP_CHRONO_HRC

namespace TGuard {



namespace TimerFunction {



    #ifdef TIME_CLOCK
    typedef long ttype;
    #endif // TIME_CLOCK
    #ifdef CV_TICK
    typedef double ttype;
    #endif // CV_TICK
    #ifdef CPP_CHRONO_HRC
    typedef std::chrono::high_resolution_clock::time_point ttype;
    #endif // CPP_CHRONO_HRC

    inline ttype getNowTime()
    {
        #ifdef TIME_CLOCK
        return clock();
        #endif // TIME_CLOCK
        #ifdef CV_TICK
        return static_cast<double>( cv::getTickCount() ) * static_cast<double>(1000) / static_cast<double>( cv::getTickFrequency() );
        #endif // CV_TICK
        #ifdef CPP_CHRONO_HRC
        return std::chrono::high_resolution_clock::now();
        #endif // CPP_CHRONO_HRC
    }

    inline std::string getTime()
    {
        time_t timet = time( nullptr );
        tm date = *localtime(&timet);
        std::stringstream ss;
        ss << (date.tm_year + 1900) << "-" << date.tm_mon << "-" << date.tm_mday << "\t" << date.tm_hour << ":" << date.tm_min << ":" << date.tm_sec ;
        return ss.str();
    }

}

// 计时器
class TimeGuard
{
    public:

        inline TimeGuard( bool AreOut, std::string TimeName):Out(AreOut),Tname(TimeName),Date(TimerFunction::getTime())
        {
            Start(NONAME);
        }

        inline TimeGuard( bool AreOut, std::string TimeName, std::string Name):Out(AreOut),Tname(TimeName),Date(TimerFunction::getTime())
        {
            Start(Name);
        }

        ~TimeGuard()
        {
            Out = false;
            Stop();
            std::fstream fout( std::string("TimeGuard.log") , std::fstream::out | std::fstream::app );
            fout << "\n\n" << std::endl;
            fout << "============================================" << std::endl;
            fout << "Now Time : " << TimerFunction::getTime() << std::endl;
            fout << Data.str() ;
            fout << "============================================" << std::endl;
            fout.close();
        }

        inline TimerFunction::ttype Start()
        {
            return Start(NONAME);
        }

        inline TimerFunction::ttype Start( std::string Name)
        {
            this->Name = Name;
            Available = true;
            TimeLast = TimerFunction::getNowTime();
            return TimeLast;
        }

        inline TimerFunction::ttype Stop()
        {
            if (Available)
            {
                TimeNow = TimerFunction::getNowTime();
                TimerFunction::ttype timems = TimeNow - TimeLast;
                Available = false;
                if (Out)
                {
                    std::cout << std::endl << Tname << " --- " << Name << " ===\t" << timems << " ms." << std::endl;
                    Data << Tname << " --- " << Name << " ===\t" << timems << " ms." << std::endl;
                }
                TimeLast = 0;
                TimeNow = 0;
                Name = NONAME;
                return timems;
            }else{
                if (Out)
                {
                    std::cout << "No Count." << std::endl;
                }
                return 0;
            }
        }

        inline TimerFunction::ttype Next()
        {
            TimerFunction::ttype time = Stop();
            Start();
            return time;
        }

        inline TimerFunction::ttype Next( std::string name)
        {
            TimerFunction::ttype time = Stop();
            Start(name);
            return time;
        }

    protected:
        bool Out;
        bool Available;
        std::string Name;
        TimerFunction::ttype TimeLast;
        TimerFunction::ttype TimeNow;
        std::string const NONAME = "No Name";
        std::string Tname;
        std::ostringstream Data;
        std::string const Date;
    private:
};

}

#endif // TIMEGUARD_H
