def Settings( **kwargs ):
    return {
        'flags': [ '-I.', '-I./deps/', '-Wall', '-Wextra', '-Werror', '-DUSE_RAX' ],
    }
