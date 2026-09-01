/*
 * kwin_borders KWin plugin
 * SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
 * SPDX-License-Identifier: GPL-3.0-or-later
*/

uniform mat4 modelViewProjectionMatrix;

in vec2 position;
in vec2 texcoord;

out vec2 v_texcoord;

void main()
{
    v_texcoord = texcoord;
    gl_Position = modelViewProjectionMatrix * vec4(position, 0.0, 1.0);
}