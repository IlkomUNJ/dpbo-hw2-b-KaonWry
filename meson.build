project('transaction-project', 'cpp',
    version: '1.0.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
        'werror=true', # Treat all warnings as errors
        'optimization=s' # Optimize for size
    ]
)

cpp_args = [
    # Rekomendasi warning untuk g++/clang
    '-Wpedantic',
    '-Wformat=2',
    '-Wundef',
    '-Wcast-align',
    '-Wwrite-strings',
    '-Wold-style-cast',
    '-Wnull-dereference',
    '-Wctor-dtor-privacy',
    '-Wno-missing-field-initializers',
]

add_global_arguments(cpp_args, language : 'cpp')

app_sources = [
  'main.cpp',
  'service/bank.cpp',
  'service/bankCustomer.cpp',
  'service/buyer.cpp',
  'service/item.cpp',
  'service/items.cpp',
  'service/seller.cpp'
]

executable('my_app',
    app_sources,
    # Example dependencies:
    # dependencies: [fmt_dep],
    include_directories: ['.', 'repo'], 
    install: true
)