__author__ = 'Orthocenter'

class Filter():
    # check if elem has all attribute and child specified in rule recursively
    def check(self, elem, rule):
        # if there is no rule specified, nothing will be filtered out
        if rule == None:
            return True

        # tag
        if elem.tag != rule.tag:
            return False

        # attribute
        for key, value in rule.items():
            if key not in elem.keys():
                return False
            if value != elem.attrib[key]:
                return False

        # each child
        for child_r in rule:
            flag_rule = False
            for child_e in elem:
                if self.check(child_e, child_r):
                    flag_rule = True
                    break
            if not flag_rule:
                return False

        return True

    # filter out against only one rule
    def filter(self, elem, rule):
        ret = []
        for child in elem:
            if self.check(child, rule):
                ret.append(child)

        return ret