add_deps("PMD")
set_languages("c++17")

target("ex_thrpool")
    set_kind("binary")
    set_default(false)
    add_files("thrpool.cc")

target("ex_pipeline")
    set_kind("binary")
    set_default(false)
    add_files("pipeline.cc")

target("ex_pipeline_parallel")
    set_kind("binary")
    set_default(false)
    add_files("pipeline_parallel.cc")