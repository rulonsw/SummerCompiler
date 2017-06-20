//
// Created by Rulon Wood on 5/29/17.
//

#ifndef SUMMERCOMPILER_DRIVER_H
#define SUMMERCOMPILER_DRIVER_H
#include<map>
#include<string>

// This is the actual driver of the compiler. This will control input flow, among other things.
// Things that are included here:
//   - References to the file being opened
//   - Read/Write control
//   - Result value
//   - Name of the file to be opened
//   - Map of variables within scope
namespace RSWCOMP {
    class Driver {
    public:
        //Con/Destructors
        Driver();
        virtual ~Driver();

        //Class variables
        std::map<std::string, int> vars;
        std::string sourceFile;
        int parse_result;
        bool trace;

        //Actual parsing method
        int parse(const std::string& sourceFile);

        // Abstract function control
        void begin_scan();
        void end_scan();


    };
}

#endif //SUMMERCOMPILER_DRIVER_H
