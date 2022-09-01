add_requires("spdlog", "fmt")
set_targetdir("dist")

if is_plat("linux") or is_plat("mingw") then
    add_syslinks("pthread")
end

target("ubox")
    set_kind("static")
    add_files("src/**.cc")
    add_packages("spdlog", "fmt", {public=true})
    add_includedirs("include", {public=true})
    set_languages("c++20")
    set_optimize("fastest")
    if is_plat("linux") then
        add_defines("__PLAT_LINUX__")
    elseif is_plat("unix") then
        add_defines("__PLAT_UNIX__")
    elseif is_plat("windows") then 
        add_defines("__PLAT_WINDOWS__")
    else then 
        add_defines("__PLAT_OTHER__")
    end


includes("examples")