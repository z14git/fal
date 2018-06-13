
from building import *

cwd     = GetCurrentDir()
src     = Glob('src/*.c') + Glob('port/*.c')
CPPPATH = [cwd + '/inc'] + [cwd + '/port']

group = DefineGroup('fal', src, depend = ['PKG_USING_FAL'], CPPPATH = CPPPATH)

Return('group')
