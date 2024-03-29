add_deps("MPD")
set_languages("c++20")
set_optimize("fastest")
set_strip("all")
set_symbols("hidden")

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

target("ex_broker")
    set_kind("binary")
    set_default(false)
    add_files("broker.cc")

target("examples")
    set_kind("phony")
    set_default(false)
    add_deps("ex_thrpool", "ex_pipeline", "ex_pipeline_parallel")
    add_deps("ex_broker")