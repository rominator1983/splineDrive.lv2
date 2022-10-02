#!/usr/bin/env python
from waflib.extras import autowaf as autowaf
import re

# Variables for 'waf dist'
APPNAME = 'splineDrive.lv2'
VERSION = '1.0.0'

# Mandatory variables
top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c compiler_cxx')
    opt.load('lv2')
    autowaf.set_options(opt)

def configure(conf):
    conf.load('compiler_c')
    conf.load('compiler_cxx')
    conf.load('lv2')
    conf.load('autowaf')

    conf.check_pkg('lv2', uselib_store='LV2')

    conf.check(features='c cxx cshlib cxxshlib', lib='m', uselib_store='M', mandatory=False)

def build(bld):
    bundle = 'splineDrive.lv2'

    # Build manifest.ttl by substitution (for portable lib extension)
    bld(features     = 'subst',
        source       = 'manifest.ttl.in',
        target       = 'lv2/%s/%s' % (bundle, 'manifest.ttl'),
        install_path = '${LV2DIR}/%s' % bundle,
        LIB_EXT      = bld.env.LV2_LIB_EXT)

    # Copy other data files to build bundle (build/splineDrive.lv2)
    for i in ['splineDrive.ttl']:
        bld(features     = 'subst',
            is_copy      = True,
            source       = i,
            target       = 'lv2/%s/%s' % (bundle, i),
            install_path = '${LV2DIR}/%s' % bundle)

    # Build plugin library
    obj = bld(features     = 'c cxx cshlib lv2lib',
              source       = 'splineDrive.cpp',
              cxxflags     = '-fPIC',
              name         = 'splineDrive',
              target       = 'lv2/%s/splineDrive' % bundle,
              install_path = '${LV2DIR}/%s' % bundle,
              uselib       = 'M LV2')
