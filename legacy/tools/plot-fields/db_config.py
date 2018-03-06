import yaml

f = open('db.yaml', 'r')
config = yaml.load(f.read())
f.close()
