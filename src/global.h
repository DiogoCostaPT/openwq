
#ifndef GLOBALH_INCLUDED
#define GLOBALH_INCLUDED

// JSON config file
class json_config
{
    public:

    struct proj_size {
        unsigned int number_of_compartments;
        unsigned int number_of_chemical_species;
        unsigned int dim_nx;
        unsigned int dim_ny;
        unsigned int dim_nz;
    } proj_size;

    struct proj_geninfo {
        std::string project_name;
        std::string geographical_location;
        std::string authors;
        std::string date;
        std::string additional_info;
    } proj_geninfo;

};


#endif