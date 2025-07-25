# Blender FLIP Fluids Add-on
# Copyright (C) 2025 Ryan L. Guy & Dennis Fassbaender
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

if "bpy" in locals():
    import importlib
    reloadable_modules = [
        'error_operators',
        'preferences_operators',
        'object_operators',
        'render_operators',
        'cache_operators',
        'world_operators',
        'export_operators',
        'material_operators',
        'preset_operators',
        'bake_operators',
        'stats_operators',
        'draw_grid_operators',
        'draw_particles_operators',
        'draw_force_field_operators',
        'helper_operators',
        'command_line_operators',
        'support_operators',
        'alembic_io_operators',
        'add_flip_objects'
    ]
    for module_name in reloadable_modules:
        if module_name in locals():
            importlib.reload(locals()[module_name])

import bpy

from . import (
        error_operators,
        preferences_operators,
        object_operators,
        render_operators,
        cache_operators,
        world_operators,
        export_operators,
        material_operators,
        preset_operators,
        bake_operators,
        stats_operators,
        draw_grid_operators,
        draw_particles_operators,
        draw_force_field_operators,
        helper_operators,
        command_line_operators,
        support_operators,
        alembic_io_operators,
        add_flip_objects
        )


def register():
    error_operators.register()
    preferences_operators.register()
    object_operators.register()
    render_operators.register()
    cache_operators.register()
    world_operators.register()
    export_operators.register()
    material_operators.register()
    preset_operators.register()
    bake_operators.register()
    stats_operators.register()
    draw_grid_operators.register()
    draw_particles_operators.register()
    draw_force_field_operators.register()
    helper_operators.register()
    command_line_operators.register()
    support_operators.register()
    alembic_io_operators.register()
    add_flip_objects.register()


def unregister():
    error_operators.unregister()
    preferences_operators.unregister()
    object_operators.unregister()
    render_operators.unregister()
    cache_operators.unregister()
    world_operators.unregister()
    export_operators.unregister()
    material_operators.unregister()
    preset_operators.unregister()
    bake_operators.unregister()
    stats_operators.unregister()
    draw_grid_operators.unregister()
    draw_particles_operators.unregister()
    draw_force_field_operators.unregister()
    helper_operators.unregister()
    command_line_operators.unregister()
    support_operators.unregister()
    alembic_io_operators.unregister()
    add_flip_objects.unregister()
