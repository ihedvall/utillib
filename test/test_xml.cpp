/*
 * Copyright 2021 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <gtest/gtest.h>
#include "util/ixmlfile.h"

using namespace util::xml;

namespace {

constexpr std::string_view kNormalXml =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<Root>\n"
    "    <Tag1>Text</Tag1>\n"
    "    <Tag2>123</Tag2>\n"
    "    <Tag3>1.23</Tag3>\n"
    "    <Tag4>True</Tag4>\n"
    "    <Tag5>False</Tag5>\n"
    "    <ChildList>\n"
    "        <Child child='Olle' order='0' condition='True'/>\n"
    "        <Child child='Pelle' order='1' condition='True'/>\n"
    "    </ChildList>\n"
    "</Root>";

constexpr std::string_view kMdComment =
    "<?xml version='1.0' encoding='UTF-8'?>\n"
    "<Root xmlns='http://www.asam.net/mdf/v4/'>\n"
    "    <Tag1 >Text</Tag1>\n"
    "    <Tag2>123</Tag2>\n"
    "    <Tag3>1.23</Tag3>\n"
    "    <Tag4>True</Tag4>\n"
    "    <Tag5>False</Tag5>\n"
    "    <ChildList>\n"
    "        <Child child='Olle' order='0' condition='True'/>\n"
    "        <Child child='Pelle' order='1' condition='True'/>\n"
    "    </ChildList>\n"
    "</Root>";

}

namespace util::test {

TEST(IXmlFile, ParseString) //NOLINT
{
  std::unique_ptr<IXmlFile> f = CreateXmlFile();

  EXPECT_TRUE(f->ParseString(kNormalXml.data()));
  EXPECT_EQ(f->Property<std::string>("Tag1"), std::string("Text"));
  EXPECT_EQ(f->Property<int>("Tag2"), 123);
  EXPECT_DOUBLE_EQ(f->Property<double>("Tag3"), 1.23);
  EXPECT_TRUE(f->Property<bool>("Tag4"));
  EXPECT_FALSE(f->Property<bool>("Tag5"));

  const auto *list = f->GetNode("ChildList");
  ASSERT_TRUE(list != nullptr);
  IXmlNode::ChildList node_list;
  list->GetChildList(node_list);
  EXPECT_EQ(node_list.size(), 2);

  auto child = node_list[1]->Attribute<std::string>("child");
  EXPECT_STREQ(child.c_str(), "Pelle");
  auto order = node_list[1]->Attribute<int>("order");
  EXPECT_EQ(order, 1);
  auto condition = node_list[1]->Attribute<bool>("condition");
  EXPECT_TRUE(condition);

  EXPECT_EQ(f->RootName(), std::string("Root"));

}

TEST(IXmlFile, ParseMdComment) //NOLINT
{
  std::unique_ptr<IXmlFile> f(std::move(CreateXmlFile()));
  EXPECT_TRUE(f->ParseString(kMdComment.data()));
  EXPECT_EQ(f->Property<std::string>("Tag1"), std::string("Text"));
  EXPECT_EQ(f->Property<int>("Tag2"), 123);
  EXPECT_DOUBLE_EQ(f->Property<double>("Tag3"), 1.23);
  EXPECT_TRUE(f->Property<bool>("Tag4"));
  EXPECT_FALSE(f->Property<bool>("Tag5"));

  const auto *list = f->GetNode("ChildList");
  ASSERT_TRUE(list != nullptr);
  IXmlNode::ChildList node_list;
  list->GetChildList(node_list);
  EXPECT_EQ(node_list.size(), 2);

  auto child = node_list[1]->Attribute<std::string>("child");
  EXPECT_STREQ(child.c_str(), "Pelle");
  auto order = node_list[1]->Attribute<int>("order");
  EXPECT_EQ(order, 1);
  auto condition = node_list[1]->Attribute<bool>("condition");
  EXPECT_TRUE(condition);

  EXPECT_EQ(f->RootName(), std::string("Root"));

}

TEST(IXmlFile, TestProperty) {
  auto writer = CreateXmlFile();
  writer->RootName("Olle");
  writer->SetProperty("TextNode", "Pelle");
  writer->SetProperty("StrNode", std::string("Olle"));
  writer->SetProperty("TrueNode", true);
  writer->SetProperty("FalseNode", false);
  writer->SetProperty("IntNode", 55);
  writer->SetProperty("FloatNode", 1.5);
  const auto xml_string = writer->WriteString(true);
  std::cout << xml_string << std::endl;

  auto reader = CreateXmlFile();
  reader->ParseString(xml_string);
  EXPECT_TRUE(reader->Property<std::string>("TextNode") == "Pelle");
  EXPECT_TRUE(reader->Property<std::string>("StrNode") == "Olle");
  EXPECT_TRUE(reader->Property<bool>("TrueNode"));
  EXPECT_FALSE(reader->Property<bool>("FalseNode"));
  EXPECT_EQ(reader->Property<int>("IntNode"), 55);
  EXPECT_EQ(reader->Property<double>("FloatNode"), 1.5);
}

TEST(IXmlNode, TestAttribute) {
  auto writer = CreateXmlFile();
  auto& root_node = writer->RootName("Olle");
  auto& temp = root_node.AddNode("Temp");

  temp.SetAttribute("TextAttr", "Pelle");
  temp.SetAttribute("StrAttr", std::string("Olle"));
  temp.SetAttribute("TrueAttr", true);
  temp.SetAttribute("FalseAttr", false);
  temp.SetAttribute("IntAttr", 55);
  temp.SetAttribute("FloatAttr", 1.5);
  const auto xml_string = writer->WriteString(true);
  std::cout << xml_string << std::endl;

  auto reader = CreateXmlFile();
  reader->ParseString(xml_string);
  const auto* temp_node = reader->GetNode("Temp");
  EXPECT_TRUE(temp_node != nullptr);

  EXPECT_TRUE(temp_node->Attribute<std::string>("TextAttr") == "Pelle");
  EXPECT_TRUE(temp_node->Attribute<std::string>("StrAttr") == "Olle");
  EXPECT_TRUE(temp_node->Attribute<bool>("TrueAttr"));
  EXPECT_FALSE(temp_node->Attribute<bool>("FalseAttr"));
  EXPECT_EQ(temp_node->Attribute<int>("IntAttr"), 55);
  EXPECT_EQ(temp_node->Attribute<double>("FloatAttr"), 1.5);
}
} // namespace util::test