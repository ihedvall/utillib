
| [Home](/index.html) | [Log](/log.html) | | [Listen](/listen.html) | [Time](/time.html) | [XML](/xml.html) |


# XML Functions
The XML function are a simple class wrapper around the common Expat library. It's much easier than the Xerces parser
but the parser is not a validating parser. It shall be used for smaller XML files or XML text snippets.

## XML File and Node objects
The XML file can either be a physical file or a string input/output. The XML is assumed to be using UTF-8 characters,
so be aware of this when presenting texts in GUI application. The file object consist of a XML node called a root tag.
The rest of the file is just more nodes (XML tags).

### XML Node 
An XML node is either holds a value or a list of other nodes. A node may have the following properties:

- **Tag Name**. The name of the node.
- **Property**. An unique node with a tag name and a value.
- **Attribute**. Is a value inside a tag.
- **Value**. The actual tag value.

Example:
~~~xml
    <Node>
        <TagName attribute='value1'>Value</TagName>
        <TagName attribute='value2'>Value</TagName>
    </Node>
~~~

