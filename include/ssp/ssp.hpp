
#ifndef SSP_SSP_HPP
#define SSP_SSP_HPP


#include "util/fs_portability.hpp"

#include <pugixml.hpp>

#include <memory>
#include <optional>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>


namespace ssp
{

struct Type
{

    std::optional<double> real;
    std::optional<int> integer;
    std::optional<bool> boolean;
    std::optional<std::string> string;

    std::optional<std::string> unit;

    [[nodiscard]] std::string typeName() const
    {
        if (isReal()) {
            return "Real";
        } else if (isInteger()) {
            return "Integer";
        } else if (isBool()) {
            return "Boolean";
        } else if (isString()) {
            return "String";
        } else {
            throw std::runtime_error("Invalid type!");
        }
    }

    [[nodiscard]] bool isReal() const { return real.has_value(); }

    [[nodiscard]] bool isInteger() const { return integer.has_value(); }

    [[nodiscard]] bool isBool() const { return boolean.has_value(); }

    [[nodiscard]] bool isString() const { return string.has_value(); }

    bool operator==(const Type& other) const
    {
        return typeName() == other.typeName();
    }
};

struct Parameter
{
    std::string name;
    Type type;
};

struct ParameterSet
{
    std::string name;
    std::vector<Parameter> parameters;
};

struct Connector
{
    std::string name;
    std::string kind;
    Type type;
};

struct Component
{
    std::string name;
    std::string source;
    std::unordered_map<std::string, Connector> connectors;
    std::unordered_map<std::string, ParameterSet> parameterSets;

    bool operator<(const Component& other) const
    {
        return name < other.name;
    }
};


struct Elements
{
    std::unordered_map<std::string, Component> components;
    std::unordered_map<std::string, std::map<Component, std::vector<Parameter>>> parameterSets;
};

struct LinearTransformation
{
    double factor{0};
    double offset{0};
};

struct Connection
{
    std::string startElement;
    std::string startConnector;

    std::string endElement;
    std::string endConnector;

    std::optional<LinearTransformation> linearTransformation;
};

struct Annotation
{
    std::string type;
    pugi::xml_node node;
};

struct DefaultExperiment
{
    std::optional<double> start;
    std::optional<double> stop;

    std::vector<Annotation> annotations;
};

struct System
{
    std::string name;
    std::string description;

    Elements elements;
    std::vector<Connection> connections;
};

class SystemStructureDescription
{

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;

public:
    const std::string& name;
    const std::string& version;

    const System& system;
    const std::optional<DefaultExperiment>& defaultExperiment;

    explicit SystemStructureDescription(const fs::path& path);

    [[nodiscard]] fs::path file(const fs::path& source) const;

    ~SystemStructureDescription();
};

} // namespace ssp


#endif // SSP_SSP_HPP