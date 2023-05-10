
// Copyright 2020, Diogo Costa, diogo.pinhodacosta@canada.ca
// This file is part of OpenWQ model.

// This program, openWQ, is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) aNCOLS later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "OpenWQ_output.hpp"

/* ########################################
// Write to Console and Log file
######################################## */

void OpenWQ_output::ConsoleLog(
    OpenWQ_wqconfig& OpenWQ_wqconfig,
    std::string& msg_string,
    bool print_console,
    bool print_logFile){

    // Print to Console if Requested (if print_console = true)
    if (print_console == true)
        std::cout << msg_string << std::endl;

    // Print to Console if Requested (if print_console = true)
    if (print_logFile == true){

        // Append line with error or warnning message
        std::ofstream log(
            OpenWQ_wqconfig.get_LogFile_name_fullpath(), 
            std::ios_base::app | std::ios_base::out);

        log << msg_string.append("\n");

    }

}