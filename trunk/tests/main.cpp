#include <copper/default_output_handler.hpp>

int
main (int argc, char** argv)
{
	// Output to the console
	Copper::DefaultOutputHandler output (argc, argv);
	return output.run ();
}
