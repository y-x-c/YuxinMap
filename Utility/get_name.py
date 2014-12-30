__author__ = 'Orthocenter'

def get_name(elem):
    # name in relation
    if elem.tag == 'member':
        if elem.attrib['type'] == 'way':
            return 'W' + elem.attrib["ref"]
        elif elem.attrib['type'] == 'node':
            return 'N' + elem.attrib["ref"]
        elif elem.attrib['type'] == 'relation':
            return 'R' + elem.attrib["ref"]
    # name in way
    elif elem.tag == 'nd':
        return 'N' + elem.attrib['ref']
    # node
    elif elem.tag == 'node':
        return 'N' + elem.attrib['id']
    # way
    elif elem.tag == 'way':
        return 'W' + elem.attrib['id']
    # relation
    elif elem.tag == 'relation':
        return 'R' + elem.attrib['id']