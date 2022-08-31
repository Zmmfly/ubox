add_requires("spdlog", "fmt")
set_targetdir("dist")

if is_plat("linux") or is_plat("mingw") then
    add_syslinks("pthread")
end

target("MPD")
    set_kind("static")
    add_files("src/**.cc")
    add_packages("spdlog", "fmt", {public=true})
    add_includedirs("include", {public=true})
    set_languages("c++20")
    set_optimize("fastest")

includes("examples")