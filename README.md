## **Simple serializer in C++**

This is header only tool, and therefore no need of complicated installation. Inspired on Boost and Cereal

It covers fundamental types and most of STL containers. Also, if a custom class is to be serialized, 
the class must have some templated member methods called serialize and deserialize and also adding serializer::oarchive & serializer::iarchive as friend classes,
so relevant elements of the custom class can be serialized.

Example with several types is shown in the git, main. 

It is not super optimized but it is functional and easy to use. 

