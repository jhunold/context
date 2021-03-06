
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>

#include <boost/assert.hpp>
#include <boost/config.hpp>

#include <boost/context/execution_context.hpp>
#include <boost/context/fixedsize.hpp>
#include <boost/context/segmented.hpp>

#ifdef BOOST_MSVC //MS VisualStudio
__declspec(noinline) void access( char *buf);
#else // GCC
void access( char *buf) __attribute__ ((noinline));
#endif
void access( char *buf)
{
  buf[0] = '\0';
}

void bar( int i)
{
    char buf[4 * 1024];

    if ( i > 0)
    {
        access( buf);
        std::cout << i << ". iteration" << std::endl;
        bar( i - 1);
    }
}

int main( int argc, char * argv[]) {
    int count = 384;

#if defined(BOOST_USE_SEGMENTED_STACKS)
    std::cout << "using segmented stacks: allocates " << count << " * 4kB == " << 4 * count << "kB on stack, ";
    std::cout << "initial stack size = " << boost::context::segmented::traits_type::default_size() / 1024 << "kB" << std::endl;
    std::cout << "application should not fail" << std::endl;
#else
    std::cout << "using standard stacks: allocates " << count << " * 4kB == " << 4 * count << "kB on stack, ";
    std::cout << "initial stack size = " << boost::context::fixedsize::traits_type::default_size() / 1024 << "kB" << std::endl;
    std::cout << "application might fail" << std::endl;
#endif

    boost::context::execution_context main_ctx(
        boost::context::execution_context::current() );

    boost::context::execution_context bar_ctx(
#if defined(BOOST_USE_SEGMENTED_STACKS)
        boost::context::segmented(),
#else
        boost::context::fixedsize(),
#endif
        [& main_ctx, count](){
            bar( count);
            main_ctx.jump_to();   
        });
    
    bar_ctx.jump_to();

    std::cout << "main: done" << std::endl;

    return 0;
}
