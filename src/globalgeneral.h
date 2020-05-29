
#ifndef GLOBALGENERALH_INCLUDED
#define GLOBALGENERALH_INCLUDED

// General information about the project
class ClassGenProj
{
    public:

    struct geninfo {
        std::string project_name;
        std::string geographical_location;
        std::string authors;
        std::string date;
        std::string additional_info;
    } geninfo;

    struct size {
        unsigned int number_of_compartments;
        unsigned int number_of_chemical_species;
        unsigned int dim_nx;
        unsigned int dim_ny;
        unsigned int dim_nz;
    } size;

};

#endif