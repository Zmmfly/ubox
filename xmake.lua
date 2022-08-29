add_requires("spdlog", "fmt")
set_targetdir("dist")

-- if is_plat("windows") then
--     add_syslinks("pthread")
-- end

target("PMD")
    set_kind("static")
    add_files("src/**.cc")
    add_packages("spdlog", "fmt", {public=true})
    add_includedirs("include", {public=true})
    set_languages("c++17")

includes("examples")