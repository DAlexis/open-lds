/*
 * macros.hpp
 *
 *  Created on: 22 Sep 2015
 *      Author: Aleksey Bulatov
 *      Mailto: AlexeyABulatov@yandex.ru
 * 
 * Copyright (c) 2015, Aleksey Bulatov
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef INCLUDE_UTILS_MACRO_HPP_
#define INCLUDE_UTILS_MACRO_HPP_

//////////////////////////////////////////////////////////////////////
// Simple C macros
//
// Convert its argument to string expanding macros if used as argument
#define STRINGIFICATE(a)    _STRINGIFICATE(a)

// Convert its argument to string without expanding macros if used as argument
#define _STRINGIFICATE(a)   #a

// Get size of static array
#define ARRAY_SIZE(a)       (sizeof(a) / sizeof(a[0]))

// Says the compiler that function argument is unused and we know it. Used to prevent 'Unused argument' warning
#define UNUSED_ARG(x)       ( (void) x )

//////////////////////////////////////////////////////////////////////
// Macros to create singleton class
//
// This macro shoud be placed at public section of class
#define SIGLETON_IN_CLASS(ClassName)    static ClassName& instance(); \
                                        ClassName(const ClassName&) = delete;

// This macro should be placed in .cpp file containing implementation of singleton class.
// Instance will be stored in a translation unit containing this macro
#define SINGLETON_IN_CPP(ClassName)     ClassName& ClassName::instance() \
                                        { \
                                                static ClassName singletonInstance; \
                                                return singletonInstance; \
                                        }

//////////////////////////////////////////////////////////////////////
// Macros to create simpliest factories
#define ABSTRACT_FACTORY(FactoryName, ProductName) \
	class FactoryName \
	{ \
	public: \
		virtual ~FactoryName() {} \
		virtual ProductName* create() = 0; \
	};

#define CONCRETE_FACTORY_HEADER(FactoryName, ProductInterface, FactoryInterface) \
	class FactoryName : public FactoryInterface \
	{ \
	public: \
		FactoryName(); \
		virtual ProductInterface* create() override; \
	};

#define CONCRETE_FACTORY_CREATE_METHOD(FactoryName, ProductName, ProductInterface) \
	ProductInterface* FactoryName::create() \
	{ \
		return new ProductName(); \
	}

#define CONCRETE_FACTORY_REGISTRATION(FactoryName, Registrator, textName) \
	FactoryName FactoryName##instance; \
	FactoryName::FactoryName() \
	{ \
		Registrator::instance().registerFactory(textName, this); \
	}


//////////////////////////////////////////////////////////////////////
// Macros to add static initializer and deinitializer to class
//
// Place it in private section of class
#define STATIC_INITIALIZER_IN_CLASS         struct StaticInitializer \
                                            { \
                                                StaticInitializer(); \
                                                ~StaticInitializer(); \
                                            }; \
                                            static StaticInitializer initializer;

// Place it in .cpp where class is implemented. This macro should be used like a function definition first line
// This code will be called when program is starting, before main() function.
#define STATIC_INIT(classname)              classname::StaticInitializer classname::initializer; \
                                            classname::StaticInitializer::StaticInitializer()

// Place it in .cpp where class is implemented. This macro should be used like a function definition first line
// This code will be called when program is exiting, after return of main() function.
#define STATIC_DEINIT(classname)            classname::StaticInitializer::~StaticInitializer()


//////////////////////////////////////////////////////////////////////
// stdexcept classes wrappers
//
// This macro creates custom exception class derived from one of stdexcept classes.
#define WRAP_STDEXCEPT(Target, Source)      class Target : public Source \
                                            { \
                                            public: \
                                                Target(const std::string&& what_arg) : \
                                                    Source(std::forward<const std::string>(what_arg)) \
                                                { } \
                                            };

// This macro should be used in class to provide exceptions inherited from stdexcept.
// It creates exceptions like YourClassName::logic_error so you can determine class that throwed it
#define STDEXCEPT_WRAPPERS                  class logic_error     : public std::logic_error \
                                            { \
                                            public: \
                                                logic_error(const std::string&& what_arg) : \
                                                    std::logic_error(std::forward<const std::string>(what_arg)) \
                                                { } \
                                            }; \
                                            class invalid_argument     : public std::invalid_argument \
                                            { \
                                            public: \
                                                invalid_argument(const std::string&& what_arg) : \
                                                    std::invalid_argument(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class domain_error     : public std::domain_error \
                                            { \
                                            public: \
                                                domain_error(const std::string&& what_arg) : \
                                                    std::domain_error(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class length_error     : public std::length_error \
                                            { \
                                            public: \
                                                length_error(const std::string&& what_arg) : \
                                                    std::length_error(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class out_of_range     : public std::out_of_range \
                                            { \
                                            public: \
                                                out_of_range(const std::string&& what_arg) : \
                                                    std::out_of_range(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class runtime_error     : public std::runtime_error \
                                            { \
                                            public: \
                                                runtime_error(const std::string&& what_arg) : \
                                                    std::runtime_error(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class range_error     : public std::range_error \
                                            { \
                                            public: \
                                                range_error(const std::string&& what_arg) : \
                                                    std::range_error(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class overflow_error     : public std::overflow_error \
                                            { \
                                            public: \
                                                overflow_error(const std::string&& what_arg) : \
                                                    std::overflow_error(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            }; \
                                            class underflow_error     : public std::underflow_error \
                                            { \
                                            public: \
                                                underflow_error(const std::string&& what_arg) : \
                                                    std::underflow_error(std::forward<const std::string>(what_arg)) \
                                                { }\
                                            };

#endif /* INCLUDE_UTILS_MACRO_HPP_ */
