/*
MIT License

Copyright (c) 2026 BatteRaquette581

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef SKELFORM_C_H
#define SKELFORM_C_H
#define SKF_VERSION "0.4.0"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief skelform_c's own boolean type, use the skf_true and skf_false constants.
 * 
 * \typedef
 */
typedef char skf_bool;
#define skf_true 1
#define skf_false 0

#ifdef _MSC_VER
#define skf_NAN (-(float)(((float)(1e+300 * 1e+300)) * 0.0F))
#else
#define skf_NAN (0.0f / 0.0f)
#endif

#define skf_Vec_BASE_CAPACITY 8
/**
 * @brief Creates a vector (dynamic array) type.
 * 
 * @attention Don't forget to free an instance with `free(vector.elements)`!
 * \def
 */
#define skf_Vec(T) struct skf_Vec_##T { \
    T *elements;                        \
    size_t capacity;                    \
    size_t size;                        \
}
/**
 * @brief Creates a vector (dynamic array) type for an skf_ namespace struct.
 * 
 * @attention Don't forget to free an instance with `free(vector.elements)`!
 * \def
 */
#define skf_Vec_struct(T) struct skf_Vec_##T { \
    struct skf_##T *elements;                  \
    size_t capacity;                           \
    size_t size;                               \
}
/**
 * @brief Appends a value to a vector (dynamic array).
 * 
 * \def
 */
#define skf_Vec_append(vec, element) {                                                  \
    if (vec.capacity <= vec.size) {                                                     \
        if (vec.capacity == 0)                                                          \
            vec.capacity = skf_Vec_BASE_CAPACITY;                                       \
        else                                                                            \
            vec.capacity <<= 1;                                                         \
        vec.elements = realloc(vec.elements, vec.capacity * sizeof(*vec.elements));     \
    }                                                                                   \
    vec.elements[vec.size++] = element;                                                 \
}
/**
 * @brief Sets the destination value to skf_true if the value is present in the vector,
 *  else skf_false.
 * 
 * \def
 */
#define skf_Vec_contains(vec, element, destination) {           \
    size_t _skf_vec_i;                                          \
    destination = skf_false;                                    \
    for (_skf_vec_i = 0; _skf_vec_i < vec.size; _skf_vec_i++) { \
        if (vec.elements[_skf_vec_i] == element) {              \
            destination = skf_true;                             \
            break;                                              \
        }                                                       \
    }                                                           \
}
skf_Vec(uint32_t);
skf_Vec(float);

/**
 * @brief Represents the color multiplier applied to the texture.
 * @attention For now, unused.
 * 
 * \struct
 */
struct skf_Tint {
    /**
     * @brief Red component (0-1).
     */
    float r;
    /**
     * @brief Green component (0-1).
     */
    float g;
    /**
     * @brief Blue component (0-1).
     */
    float b;
    /**
     * @brief Alpha (opacity) component (0-1).
     */
    float a;
};

/**
 * @brief Represents a 2 dimensional position or spatial vector.
 * 
 * \struct
 */
struct skf_Vec2 {
    /**
     * @brief X component.
     */
    float x;
    /**
     * @brief Y component.
     */
    float y;
};

/**
 * @brief Represents a vertex in a bone non-quad mesh.
 * 
 * \struct
 */
struct skf_Vertex {
    /**
     * @brief Vertex position.
     */
    struct skf_Vec2 pos;
    /**
     * @brief Vertex texture UV coordinate (0-1 for both X and Y).
     */
    struct skf_Vec2 uv;
    /**
     * @brief Initial vertex position (left untouched by functions).
     */
    struct skf_Vec2 init_pos;
};
skf_Vec_struct(Vertex);

/*!
 * @brief Represents a vertex in a bone non-quad mesh.
 * @attention For now, unused.
 */
enum skf_HandlePreset {
    skf_HandlePreset_Linear,
    skf_HandlePreset_SineIn,
    skf_HandlePreset_SineOut,
    skf_HandlePreset_SineInOut,
    skf_HandlePreset_None,
    skf_HandlePreset_Custom
};

/**
 * @brief Represents an animation keyframe, depicting which value to modify, when and how.
 * 
 * \struct
 */
struct skf_Keyframe {
    /**
     * @brief Handle to use for start of interpolation.
     */
    struct skf_Vec2 start_handle;
    /**
     * @brief Handle to use for end of interpolation.
     */
    struct skf_Vec2 end_handle;
    /**
     * @brief Element to be animated by the keyframe.
     */
    const char *element;
    /**
     * @brief Value to be modified by the keyframe.
     */
    const char *value_str;
    /**
     * @brief Frame when the keyframe starts.
     */
    uint32_t frame;
    /**
     * @brief ID of bone that keyframe refers to.
     */
    uint32_t bone_id;
    /**
     * @brief Value to set to.
     */
    float value;
    /**
     * @brief Legacy member, left in for compatibility (to be removed).
     * @deprecated
     */
    float label_top;
    /**
     * @brief Handle preset to be used.
     * @attention For now, unused.
     */
    enum skf_HandlePreset handle_preset;
};
skf_Vec_struct(Keyframe);

/**
 * @brief Represents an animation, containing many keyframes, a name, and a framerate.
 * 
 * \struct
 */
struct skf_Animation {
    /**
     * @brief Vector of animation keyframes.
     */
    struct skf_Vec_Keyframe keyframes;
    /**
     * @brief Animation name.
     */
    const char *name;
    /**
     * @brief Animation framerate.
     */
    uint32_t fps;
};
skf_Vec_struct(Animation);

/**
 * @brief A bone bind vertex, only containing a weight.
 * 
 * \struct
 */
struct skf_BoneBindVert {
    /**
     * @brief Vertex ID.
     */
    uint32_t id;
    /**
     * @brief Vertex weight.
     */
    float weight;
};
skf_Vec_struct(BoneBindVert);

/**
 * @brief A bone bind, containing many vertices.
 * 
 * \struct
 */
struct skf_BoneBind {
    struct skf_Vec_BoneBindVert verts;
    int32_t bone_id;
    /**
     * @brief skf_true i.
     */
    skf_bool is_path;
};
skf_Vec_struct(BoneBind);

/**
 * @brief Represents a bone, capable of inverse kinematics, binds,
 *  and possibly a mesh (as well as other information).
 * 
 * \struct
 */
struct skf_Bone {
    /**
     * @brief Vector of IDs of bones for inverse kinematics.
     */
    struct skf_Vec_uint32_t ik_bone_ids;
    /**
     * @brief Vector of vertices, if the bone is a custom mesh.
     *  If its size is 0, it is a quad mesh, with the scale
     *  and texture size used for the vertices' positions.
     */
    struct skf_Vec_Vertex vertices;
    /**
     * @brief If the bone is a custom mesh, this represents the
     *  indices of the mesh triangles using vertices' IDs.
     */
    struct skf_Vec_uint32_t indices;
    /**
     * @brief Vector of bone binds.
     */
    struct skf_Vec_BoneBind binds;
    /**
     * @brief Tint to multiply the color's texture to.
     * @attention For now, unused.
     */
    struct skf_Tint tint;
    /**
     * @brief Initial tint to multiply the color's texture to
     *  (left untouched by runtimes).
     * @attention For now, unused.
     */
    struct skf_Tint init_tint;
    /**
     * @brief Scale to multiply the bone-space vertex positions to.
     */
    struct skf_Vec2 scale;
    /**
     * @brief Vertex position, can be modified by a runtime to fit scale.
     */
    struct skf_Vec2 pos;
    /**
     * @brief Initial scale to multiply the bone-space vertex positions
     *  to (left untouched by runtimes).
     */
    struct skf_Vec2 init_scale;
    /**
     * @brief Initial vertex position (left untouched by runtimes).
     */
    struct skf_Vec2 init_pos;
    /**
     * @brief Name of the bone in the editor.
     */
    const char *name;
    /**
     * @brief Name of the texture.
     */
    const char *tex;
    /**
     * @brief Initial name of the texture (left untouched by runtimes).
     */
    const char *init_tex;
    /**
     * @brief The family's constraint.
     */
    const char *ik_constraint;
    /**
     * @brief The family's initial constraint (left untouched by runtimes).
     */
    const char *init_ik_constraint;
    /**
     * @brief The inverse kinematics mode, how inverse kinematics should be
     *  performed, can be "FABRIK", or "ARC".
     */
    const char *ik_mode;
    /**
     * @brief The initial inverse kinematics mode (left untouched by runtimes),
     *  how inverse kinematics should be performed, can be "FABRIK", or "ARC".
     */
    const char *init_ik_mode;
    /**
     * @brief Unique bone ID.
     */
    uint32_t id;
    /**
     * @brief Bone ID of the parent of this bone (-1 if no parent).
     */
    int32_t parent_id;
    /**
     * @brief Family ID of this bone (-1 if no family).
     */
    int32_t ik_family_id;
    /**
     * @brief Target ID of this bone (-1 if no target).
     */
    int32_t ik_target_id;
    /**
     * @brief Z index, higher means higher priority and rendered later.
     */
    int32_t zindex;
    /**
     * @brief Initial Z index (left untouched by runtimes), higher means
     *  higher priority and rendered later.
     */
    int32_t init_zindex;
    /**
     * @brief Rotation in radians.
     */
    float rot;
    /**
     * @brief Initial rotation in radians (left untouched by runtimes).
     */
    float init_rot;
    /**
     * @brief skf_true if hidden, else skf_false.
     */
    skf_bool hidden;
    /**
     * @brief skf_true if hidden, else skf_false, initial state left
     *  untouched by runtimes.
     */
    skf_bool init_hidden;
};
skf_Vec_struct(Bone);

/**
 * @brief Represents a rectangle for a texture in an atlas, and its
 *  name.
 * 
 * \struct
 */
struct skf_Texture {
    /**
     * @brief Top left corner of the texture rectangle.
     */
    struct skf_Vec2 offset;
    /**
     * @brief Size the texture rectangle.
     */
    struct skf_Vec2 size;
    /**
     * @brief Texture name.
     */
    const char *name;
    /**
     * @brief Index of the atlas it corresponds to.
     */
    uint32_t atlas_idx;
};
skf_Vec_struct(Texture);

/**
 * @brief A style can swap an armature's textures.
 * 
 */
struct skf_Style {
    /**
     * @brief Vector containing the textures.
     */
    struct skf_Vec_Texture textures;
    /**
     * @brief Style name.
     */
    const char *name;
    /**
     * @brief Style unique ID.
     */
    uint32_t id;
    /**
     * @brief skf_true if active, else skf_false.
     */
    skf_bool active;
};
skf_Vec_struct(Style);

/**
 * @brief Represents a texture atlas, with a size and file name
 *  in the archive.
 * 
 * \struct
 */
struct skf_TexAtlas {
    /**
     * @brief Size in pixels of the atlas.
     */
    struct skf_Vec2 size;
    /**
     * @brief File name of the atlas in the archive.
     */
    const char *filename;
};
skf_Vec_struct(TexAtlas);

/**
 * @brief Represents an armature (an .skf file), with bones,
 *  animations, textures, style, and texture atlases.
 * @attention All vectors must be freed here with
 *  `free(vector.elements)`.
 * 
 * \struct
 */
struct skf_Armature {
    /**
     * @brief Vector of bone IDs that contain inverse kinematics
     *  data.
     */
    struct skf_Vec_uint32_t ik_root_ids;
    /**
     * @brief Vector of bones present in the armature.
     */
    struct skf_Vec_Bone bones;
    /**
     * @brief Vector of animations present in the armature.
     */
    struct skf_Vec_Animation animations;
    /**
     * @brief Vector of textures present in the armature.
     */
    struct skf_Vec_Texture textures;
    /**
     * @brief Vector of textures present in the armature.
     */
    struct skf_Vec_Style styles;
    /**
     * @brief Vector of textures atlases present in the armature.
     */
    struct skf_Vec_TexAtlas atlases;
    /**
     * @brief skf_true if the inverse kinematics are baked, else
     *  skf_false.
     */
    skf_bool baked_ik;
};

struct skf_inverse_kinematics_rotation {
    uint32_t ik_bone_id;
    float rotation;
};
skf_Vec_struct(inverse_kinematics_rotation);


size_t skf_get_prev_frame(
    const struct skf_Vec_Keyframe *keyframes,
    const uint32_t frame, 
    const char *element,
    const uint32_t id
)
{
    size_t i, prev = SIZE_MAX;
    for (i = 0; i < keyframes->size; i++) {
        const struct skf_Keyframe *kf = &keyframes->elements[i];
        if (kf->frame <= frame && strcmp(kf->element, element) == 0 && kf->bone_id == id)
            prev = i;
    }
    return prev;
}

size_t skf_get_next_frame(
    const struct skf_Vec_Keyframe *keyframes,
    const uint32_t frame, 
    const char *element,
    const uint32_t id
)
{
    size_t i;
    for (i = 0; i < keyframes->size; i++) {
        const struct skf_Keyframe *kf = &keyframes->elements[i];
        if (kf->frame > frame && strcmp(kf->element, element) == 0 && kf->bone_id == id)
            return i;
    }
    return SIZE_MAX;
}

float skf_cubic_bezier(const float t, const float p1, const float p2)
{
    const float u = 1.0f - t;
    return 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t;
}

float skf_cubic_bezier_derivative(const float t, const float p1, const float p2)
{
    const float u = 1.0f - t;
    return 3.0f * u * u * p1 + 6.0f * u * t * (p2 - p1) + 3.0f * t * t * (1.0f - p2);
}

/* https://stackoverflow.com/a/16659263 */
float skf_clamp(float d, float min, float max)
{
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

#define SKF_NEWTON_RAPHSON_EPSILON 0.00001f
float skf_interpolate(
    const uint32_t current,
    const uint32_t max,
    const float start_val,
    const float end_val,
    const struct skf_Vec2 *start_handle,
    const struct skf_Vec2 *end_handle
)
{
    /* snapping behavior for None transition preset */
    if (start_handle->y == 999.0f && end_handle->y == 999.0f)
        return start_val;
    if (max == 0 || current >= max)
        return end_val;
    
    /* solve for time (x axis) with Newton-Raphson */
    {
        const float initial = ((float) current) / ((float) max);
        float t = initial;
        size_t i;
        for (i = 0; i < 5; i++) {
            const float x = skf_cubic_bezier(t, start_handle->x, end_handle->x);
            const float dx = skf_cubic_bezier_derivative(t, start_handle->x, end_handle->x);
            if (fabsf(dx) < SKF_NEWTON_RAPHSON_EPSILON)
                break;
            t -= (x - initial) / dx;
            t = skf_clamp(t, 0.0f, 1.0f);
        }

        {
            const float progress = skf_cubic_bezier(t, start_handle->y, end_handle->y);
            return start_val + (end_val - start_val) * progress;
        }
    }
}

/* Interpolate an f32 value from the specified keyframe data. */
void skf_interpolate_keyframes(
    const char *element,
    float *field,
    const struct skf_Vec_Keyframe *keyframes,
    const uint32_t id,
    const uint32_t frame,
    const uint32_t blend_frames
)
{
    size_t prev = skf_get_prev_frame(keyframes, frame, element, id);
    size_t next = skf_get_next_frame(keyframes, frame, element, id);

    /* ensure both frames are pointing somewhere */
    if (prev == SIZE_MAX) {
        prev = next;
    } else if (next == SIZE_MAX) {
        next = prev;
    }

    /* if both are max, then the frame doesn't exist. Fallback to init value */
    if (prev == SIZE_MAX && next == SIZE_MAX) {
        return;
    }

    {
        const uint32_t total_frames = keyframes->elements[next].frame - keyframes->elements[prev].frame;
        const uint32_t current_frame = frame - keyframes->elements[prev].frame;

        const float result = skf_interpolate(
            current_frame,
            total_frames,
            keyframes->elements[prev].value,
            keyframes->elements[next].value,
            &keyframes->elements[next].start_handle,
            &keyframes->elements[next].end_handle
        );

        const struct skf_Vec2 z = {0.0f, 0.0f};
        *field = skf_interpolate(current_frame, blend_frames, *field, result, &z, &z);
    }
}

#define skf_interpolate_bone_next_frame(element, field, kf_value) {       \
    prev_frame = skf_get_prev_frame(keyframes, frame, element, bone->id); \
    if (prev_frame != SIZE_MAX) {                                         \
        field = keyframes->elements[prev_frame].kf_value;                 \
    }                                                                     \
}
#define skf_interpolate_bone_next_frame_string(element, field) {          \
    prev_frame = skf_get_prev_frame(keyframes, frame, element, bone->id); \
    if (prev_frame != SIZE_MAX) {                                         \
        field = strdup(keyframes->elements[prev_frame].value_str);        \
    }                                                                     \
}
void skf_interpolate_bone(
    struct skf_Bone *bone,
    const struct skf_Vec_Keyframe *keyframes,
    const uint32_t bone_id,
    const uint32_t frame,
    const uint32_t blend_frame
)
{
    size_t prev_frame;
    skf_interpolate_keyframes(
        "PositionX",
        &bone->pos.x,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "PositionY",
        &bone->pos.y,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "Rotation",
        &bone->rot,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "ScaleX",
        &bone->scale.x,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "ScaleY",
        &bone->scale.y,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "TintR",
        &bone->tint.r,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "TintG",
        &bone->tint.g,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "TintB",
        &bone->tint.b,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_keyframes(
        "TintA",
        &bone->tint.a,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    skf_interpolate_bone_next_frame_string("Texture", bone->tex);
    skf_interpolate_bone_next_frame       ("Zindex", bone->zindex, value);
    skf_interpolate_bone_next_frame_string("IkMode", bone->ik_mode);
    skf_interpolate_bone_next_frame_string("IkConstraint", bone->ik_constraint);
}

skf_bool skf_is_animated(
    const uint32_t bone_id,
    const char *el, 
    const struct skf_Vec_Animation *anims
)
{
    size_t anim_i, kf_i;
    for (anim_i = 0; anim_i < anims->size; anim_i++) {
        const struct skf_Animation *anim = &anims->elements[anim_i];
        for (kf_i = 0; kf_i < anim->keyframes.size; kf_i++) {
            const struct skf_Keyframe *kf = &anim->keyframes.elements[kf_i];
            if (kf->bone_id == bone_id && strcmp(kf->element, el) == 0)
                return skf_true;
        }
    }
    return skf_false;
}

#define skf_reset_bone_interpolate(element, field, init_field) {                \
    if (!skf_is_animated(bone->id, element, anims))                             \
        field = skf_interpolate(frame, blend_frame, field, init_field, &z, &z); \
}
#define skf_reset_bone_hard_interpolate(element, field, init_field) { \
    if (!skf_is_animated(bone->id, element, anims))                   \
        field = init_field;                                           \
}
#define skf_reset_bone_hard_interpolate_string(element, field, init_field) { \
    if (!skf_is_animated(bone->id, element, anims))                          \
        field = strdup(init_field);                                          \
}
void skf_reset_bone(
    struct skf_Bone *bone,
    const uint32_t frame,
    const uint32_t blend_frame,
    const struct skf_Vec_Animation *anims
)
{
    const struct skf_Vec2 z = {0.0f, 0.0f};
    skf_reset_bone_interpolate            ("PositionX", bone->pos.x, bone->init_pos.x);
    skf_reset_bone_interpolate            ("PositionY", bone->pos.y, bone->init_pos.y);
    skf_reset_bone_interpolate            ("Rotation", bone->rot, bone->init_rot);
    skf_reset_bone_interpolate            ("ScaleX", bone->scale.x, bone->init_scale.x);
    skf_reset_bone_interpolate            ("ScaleY", bone->scale.y, bone->init_scale.y);
    skf_reset_bone_hard_interpolate_string("Texture", bone->tex, bone->init_tex);
    skf_reset_bone_hard_interpolate       ("Zindex", bone->zindex, bone->init_zindex);
    skf_reset_bone_hard_interpolate       ("Hidden", bone->hidden, bone->init_hidden);
    skf_reset_bone_hard_interpolate_string("IkMode", bone->ik_mode, bone->init_ik_mode);
    skf_reset_bone_hard_interpolate_string("IkConstraint", bone->ik_constraint, bone->init_ik_constraint);
}

/**
 * @brief Interpolates bone fields based on provided animation data, as well as initial states
 *  non-animated fields.
 * 
 * @param bones Pointer to the vector containing the bones of the armature.
 * @param anims Pointer to the vector containing the animations in use.
 * @param frames Pointer to the vector containing the respective animation frames.
 * @param blend_frames Pointer to the vector containing the respective animation "blend"
 *  cross-fade frames.
 * @return This function is an in-place function and will directly modify the bones vector.
 */
void skf_animate(
    struct skf_Vec_Bone *bones,
    const struct skf_Vec_Animation *anims,
    const struct skf_Vec_uint32_t *frames,
    const struct skf_Vec_uint32_t *blend_frames
)
{
    size_t a, b, bone_i;
    for (a = 0; a < anims->size; a++) {
        for (b = 0; b < bones->size; b++) {
            skf_interpolate_bone(
                &bones->elements[b],
                &anims->elements[a].keyframes,
                bones->elements[b].id,
                frames->elements[a],
                blend_frames->elements[a]
            );
        }
    }

    for (bone_i = 0; bone_i < bones->size; bone_i++) {
        skf_reset_bone(
            &bones->elements[bone_i],
            frames->elements[0],
            blend_frames->elements[0],
            anims
        );
    }
}

struct skf_Bone skf_bone_shallow_copy(const struct skf_Bone *bone)
{
    struct skf_Bone new_bone;
    memcpy(&new_bone, bone, sizeof(struct skf_Bone));
    return new_bone;
}

struct skf_Vec2 skf_vec2_add(const struct skf_Vec2 v1, const struct skf_Vec2 v2)
{
    struct skf_Vec2 new_vec;
    new_vec.x = v1.x + v2.x;
    new_vec.y = v1.y + v2.y;
    return new_vec;
}

struct skf_Vec2 skf_vec2_sub(const struct skf_Vec2 v1, const struct skf_Vec2 v2)
{
    struct skf_Vec2 new_vec;
    new_vec.x = v1.x - v2.x;
    new_vec.y = v1.y - v2.y;
    return new_vec;
}

struct skf_Vec2 skf_vec2_mul(const struct skf_Vec2 v1, const struct skf_Vec2 v2)
{
    struct skf_Vec2 new_vec;
    new_vec.x = v1.x * v2.x;
    new_vec.y = v1.y * v2.y;
    return new_vec;
}

struct skf_Vec2 skf_vec2_div(const struct skf_Vec2 v1, const struct skf_Vec2 v2)
{
    struct skf_Vec2 new_vec;
    new_vec.x = v1.x / v2.x;
    new_vec.y = v1.y / v2.y;
    return new_vec;
}

float skf_vec2_magnitude(const struct skf_Vec2 vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

struct skf_Vec2 skf_vec2_normalize(const struct skf_Vec2 vec)
{
    float magnitude = skf_vec2_magnitude(vec);
    struct skf_Vec2 new_vec;
    if (magnitude == 0.0f) {
        new_vec.x = 0.0f;
        new_vec.y = 0.0f;
    } else {
        new_vec.x = vec.x / magnitude;
        new_vec.y = vec.y / magnitude;
    }
    return new_vec;
}

struct skf_Vec2 skf_vec2_rotate(const struct skf_Vec2 vec, const float radians)
{
    struct skf_Vec2 new_vec;
    new_vec.x = vec.x * cos(radians) - vec.y * sin(radians);
    new_vec.y = vec.x * sin(radians) + vec.y * cos(radians);
    return new_vec;
}

float skf_vec_inverse_kinematics_rotation_get(
    const struct skf_Vec_inverse_kinematics_rotation *ik_rots,
    const uint32_t ik_bone_id
)
{
    size_t i;
    for (i = 0; i < ik_rots->size; i++) {
        const struct skf_inverse_kinematics_rotation *ik_rot = &ik_rots->elements[i];
        if (ik_rot->ik_bone_id == ik_bone_id)
            return ik_rot->rotation;
    }
    return skf_NAN;
}

void skf_inheritance(
    struct skf_Vec_Bone *bones,
    const struct skf_Vec_inverse_kinematics_rotation *ik_rots
)
{   
    size_t b;
    for (b = 0; b < bones->size; b++) {
        struct skf_Bone *bone = &bones->elements[b];
        const float ik_rot = skf_vec_inverse_kinematics_rotation_get(ik_rots, (uint32_t) b);
        if (bone->parent_id != -1) {
            const struct skf_Bone *parent = &bones->elements[bone->parent_id];
            bone->rot += parent->rot;
            bone->scale = skf_vec2_mul(bone->scale, parent->scale);
            bone->pos = skf_vec2_mul(bone->pos, parent->scale);
            bone->pos = skf_vec2_rotate(bone->pos, parent->rot);
            bone->pos = skf_vec2_add(bone->pos, parent->pos);
        }

        if (ik_rot == ik_rot) /* !isnan(ik_rot) */
            bone->rot = ik_rot;
    }
}

void skf_fabrik(
    struct skf_Vec_Bone *bones,
    const struct skf_Vec2 root,
    const struct skf_Vec2 target
)
{
    struct skf_Vec2 next_pos = target;
    float next_length = 0.0f;
    struct skf_Vec2 prev_pos = root;
    float prev_length = 0.0f;
    size_t b, b_reversed;
    for (b = 1; b <= bones->size; b++) {
        b_reversed = bones->size - b;
        {
            const struct skf_Vec2 dv = skf_vec2_normalize(
                skf_vec2_sub(next_pos, bones->elements[b_reversed].pos));
            struct skf_Vec2 next_length_vector;
            next_length_vector.x = next_length;
            next_length_vector.y = next_length;
            {
                const struct skf_Vec2 length = skf_vec2_mul(dv, next_length_vector);
                if (b_reversed != 0) {
                    next_length = skf_vec2_magnitude(skf_vec2_sub(
                        bones->elements[b_reversed].pos, bones->elements[b_reversed - 1].pos));
                }
                next_pos = bones->elements[b_reversed].pos = skf_vec2_sub(next_pos, length);
            }
        }
    }

    /* backward-reaching */
    for (b = 0; b <= bones->size; b++) {
        const struct skf_Vec2 dv = skf_vec2_normalize(
            skf_vec2_sub(prev_pos, bones->elements[b].pos));
        struct skf_Vec2 prev_length_vector;
        prev_length_vector.x = prev_length;
        prev_length_vector.y = prev_length;
        {
            const struct skf_Vec2 length = skf_vec2_mul(dv, prev_length_vector);
            if (b != bones->size - 1) {
                prev_length = skf_vec2_magnitude(skf_vec2_sub(
                    bones->elements[b].pos, bones->elements[b + 1].pos));
            }
            prev_pos = bones->elements[b].pos = skf_vec2_sub(prev_pos, length);
        }
    }
}

void skf_arc_ik(
    struct skf_Vec_Bone *bones,
    const struct skf_Vec2 root,
    const struct skf_Vec2 target
)
{
    /* determine where bones will be on the arc line (ranging from 0 to 1) */
    struct skf_Vec_float dist = {0};
    const float max_length = skf_vec2_magnitude(skf_vec2_sub(bones->elements[bones->size - 1].pos, root));
    float curr_length = 0.0f;
    size_t b;
    skf_Vec_append(dist, 0.0f);

    for (b = 1; b < bones->size; b++) {
        const float length = skf_vec2_magnitude(skf_vec2_sub(
            bones->elements[b].pos, bones->elements[b - 1].pos));
        curr_length += length;
        skf_Vec_append(dist, curr_length / max_length);
    }

    {
        const struct skf_Vec2 base = skf_vec2_sub(target, root);
        const float base_angle = atan2(base.y, base.x);
        float base_mag = skf_vec2_magnitude(base);
        if (base_mag > max_length)
            base_mag = max_length;

        {
            const float peak = max_length / base_mag;
            const float valley = base_mag / max_length;

            for (b = 1; b < bones->size; b++) {
                bones->elements[b].pos.x *= valley;
                bones->elements[b].pos.y = root.y + (1.0f - peak) 
                    * sin(dist.elements[b] * 3.141592f) * base_mag;

                {
                    const struct skf_Vec2 rotated = skf_vec2_rotate(skf_vec2_sub(
                        bones->elements[b].pos, root), base_angle);
                    bones->elements[b].pos = skf_vec2_add(rotated, root);
                }
            }

            free(dist.elements);
        }
    }
}

void skf_point_bones(
    struct skf_Vec_Bone *bones,
    const struct skf_Bone *family
)
{
    const size_t ik_bones_size = family->ik_bone_ids.size;
    const struct skf_Bone *end_bone = &bones->elements[family->ik_bone_ids.elements
        [ik_bones_size - 1]];
    struct skf_Vec2 tip_pos = end_bone->pos;
    size_t i, i_reversed;
    for (i = 1; i <= ik_bones_size; i++) {
        i_reversed = ik_bones_size - i;
        if (i_reversed == ik_bones_size - 1) {
            continue;
        }
        {
            struct skf_Bone *bone = &bones->elements[
                family->ik_bone_ids.elements[i_reversed]];

            const struct skf_Vec2 dir = skf_vec2_sub(tip_pos, bone->pos);
            bone->rot = atan2(dir.y, dir.x);
            tip_pos = bone->pos;
        }
    }
}

void skf_apply_constraints(
    struct skf_Vec_Bone *bones,
    const struct skf_Bone *family
)
{
    const struct skf_Vec2 root = bones->elements[family->ik_bone_ids.elements[0]].pos;
    const struct skf_Vec2 target = bones->elements[family->ik_target_id].pos;
    const struct skf_Vec2 joint_dir = skf_vec2_normalize(skf_vec2_sub(
        bones->elements[family->ik_bone_ids.elements[1]].pos, root));
    const struct skf_Vec2 base_dir = skf_vec2_normalize(skf_vec2_sub(target, root));
    const float dir = joint_dir.x * base_dir.y - base_dir.x * joint_dir.y;
    const float base_angle = atan2(base_dir.y, base_dir.x);

    const skf_bool cw = strcmp(family->ik_constraint, "Clockwise") == 0 && dir > 0.0f;
    const skf_bool ccw = strcmp(family->ik_constraint, "CounterClockwise") == 0 && dir < 0.0f;
    if (ccw || cw) {
        size_t i;
        for (i = 0; i < family->ik_bone_ids.size; i++) {
            const uint32_t ik_bone_id = family->ik_bone_ids.elements[i];
            bones->elements[ik_bone_id].rot = -bones->elements[ik_bone_id].rot
                + base_angle + base_angle;
        }
    }
}

struct skf_Vec_inverse_kinematics_rotation skf_inverse_kinematics(
    struct skf_Vec_Bone *bones,
    const struct skf_Vec_uint32_t *ik_root_ids
)
{
    struct skf_Vec_inverse_kinematics_rotation ik_rots = {0};
    size_t i, b, fabrik_step;
    for (i = 0; i < ik_root_ids->size; i++) {
        const struct skf_Bone *family = &bones->elements[ik_root_ids->elements[i]];
        if (family->ik_target_id == -1)
            continue;

        {
            const struct skf_Vec2 root = bones->elements[family->ik_bone_ids.elements[0]].pos;
            const struct skf_Vec2 target = bones->elements[family->ik_target_id].pos;
            struct skf_Vec_Bone family_bones = {0};
            for (b = 0; b < bones->size; b++) {
                const struct skf_Bone *bone = &bones->elements[b];
                skf_bool contains;
                skf_Vec_contains(family->ik_bone_ids, bone->id, contains);
                if (contains)
                    skf_Vec_append(family_bones, *bone);
            }

            if (family_bones.size > 0) {
                if (strcmp(family->ik_mode, "FABRIK") == 0)
                    for (fabrik_step = 0; fabrik_step < 10; fabrik_step++)
                        skf_fabrik(&family_bones, root, target);
                else
                    skf_arc_ik(&family_bones, root, target);

                for (b = 0; b < family_bones.size; b++) {
                    uint32_t bone_id = family_bones.elements[b].id;
                    bones->elements[bone_id].pos = family_bones.elements[b].pos;
                }
            }

            skf_point_bones(bones, family);
            skf_apply_constraints(bones, family);

            for (b = 0; b < family->ik_bone_ids.size; b++) {
                if (b == family->ik_bone_ids.size - 1)
                    continue;
                {
                    struct skf_inverse_kinematics_rotation entry;
                    entry.ik_bone_id = family->ik_bone_ids.elements[b];
                    entry.rotation = bones->elements[
                        family->ik_bone_ids.elements[b]].rot;
                    skf_Vec_append(ik_rots, entry);
                }
            }

            free(family_bones.elements);
        }
    }

    return ik_rots;
}

struct skf_Vec2 skf_inherit_vert(
    struct skf_Vec2 *pos,
    const struct skf_Bone *bone
) {
    *pos = skf_vec2_mul(*pos, bone->scale);
    *pos = skf_vec2_rotate(*pos, bone->rot);
    *pos = skf_vec2_add(*pos, bone->pos);
    return *pos;
}

void skf_construct_verts(struct skf_Vec_Bone *bones)
{
    size_t b;
    for (b = 0; b < bones->size; b++) {
        struct skf_Bone *bone = &bones->elements[b];

        /* move vertex to main bone. */
        /* this will be overridden if vertex has a bind. */
        size_t vertex_i, bi;
        for (vertex_i = 0; vertex_i < bone->vertices.size; vertex_i++) {
            bone->vertices.elements[vertex_i].pos =
                skf_inherit_vert(&bone->vertices.elements[vertex_i].pos, bone);
        }

        for (bi = 0; bi < bone->binds.size; bi++) {
            size_t bbi, v;
            const int32_t b_id = bone->binds.elements[bi].bone_id;
            if (b_id == -1)
                continue;

            {
                struct skf_Bone *bind_bone;
                struct skf_BoneBind *bind;
                for (bbi = 0; bbi < bones->size; bbi++) {
                    struct skf_Bone *bb = &bones->elements[bbi];
                    if (bb->id == (uint32_t) b_id) {
                        bind_bone = bb;
                        break;
                    }
                }
                bind = &bones->elements[b].binds.elements[bi];
                for (v = 0; v < bind->verts.size; v++) {
                    struct skf_Bone *prev_bone = NULL, *next_bone = NULL;
                    struct skf_Vec_BoneBind *binds;
                    size_t bone_i, prev, next;
                    const uint32_t vert_id = bind->verts.elements[v].id;

                    if (!bind->is_path) {
                        /* weights */
                        struct skf_Vertex *vert =
                            &bones->elements[b].vertices.elements[vert_id];
                        const float weight = bind->verts.elements[v].weight;
                        const struct skf_Vec2 end_pos = skf_vec2_sub(
                            skf_inherit_vert(&vert->init_pos, bind_bone),
                            vert->pos
                        );
                        struct skf_Vec2 weight_vec;
                        weight_vec.x = weight;
                        weight_vec.y = weight;
                        vert->pos = skf_vec2_add(vert->pos,
                            skf_vec2_mul(end_pos, weight_vec));
                        continue;
                    }

                    /* pathing:
                    Bone binds are treated as one continuous line.
                    Vertices will follow along this path.

                    get previous and next bone*/
                    binds = &bones->elements[b].binds;
                    prev = bi;
                    if (prev > 0)
                        prev--;
                    next = bi + 1;
                    if (next >= binds->size)
                        next = binds->size - 1;
                    for (bone_i = 0; bone_i < bones->size; bone_i++) {
                        struct skf_Bone *b = &bones->elements[bone_i];
                        if (b->id ==
                            (uint32_t) binds->elements[prev].bone_id) {
                            prev_bone = b;
                        } else if (b->id ==
                            (uint32_t) binds->elements[next].bone_id) {
                            next_bone = b;
                        }
                        if (prev_bone != NULL && next_bone != NULL)
                            break;
                    }

                    {
                        /* get the average of normals between previous
                        bone, this bone, and next bone */
                        struct skf_Vertex *vert;
                        const struct skf_Vec2 prev_dir = skf_vec2_sub(
                            bind_bone->pos, prev_bone->pos);
                        const struct skf_Vec2 next_dir = skf_vec2_sub(
                            next_bone->pos, bind_bone->pos);
                        struct skf_Vec2 prev_normal, next_normal, average,
                            rotated, weight_vec;
                        float normal_angle, weight;
                        prev_normal.x = -prev_dir.y;
                        prev_normal.y = prev_dir.x;
                        prev_normal = skf_vec2_normalize(prev_normal);
                        next_normal.x = -next_dir.y;
                        next_normal.y = next_dir.x;
                        next_normal = skf_vec2_normalize(next_normal);
                        average = skf_vec2_add(prev_normal, next_normal);
                        normal_angle = atan2(average.y, average.x);

                        /* move vertex to bind bone, then just adjust it to
                        'bounce' off the line's surface */
                        vert = &bones->elements[b].vertices.elements[vert_id];
                        vert->pos = skf_vec2_add(vert->init_pos, bind_bone->pos);
                        rotated = skf_vec2_rotate(
                            skf_vec2_sub(vert->pos, bind_bone->pos),
                            normal_angle
                        );
                        weight = bind->verts.elements[v].weight;
                        weight_vec.x = weight;
                        weight_vec.y = weight;
                        vert->pos = skf_vec2_add(
                            bind_bone->pos, skf_vec2_mul(rotated, weight_vec)
                        );
                    }
                }
            }
        }
    }
}

/**
 * @brief Constructs the bones and vertices with inverse kinematics.
 * 
 * @param armature Pointer to the armature.
 * @return New modified bones.
 */
struct skf_Vec_Bone skf_construct(struct skf_Armature *armature)
{
    size_t i;
    struct skf_Vec_Bone bones = {0};
    struct skf_Vec_inverse_kinematics_rotation ik_rots = {0};
    for (i = 0; i < armature->bones.size; i++)
        skf_Vec_append(bones, skf_bone_shallow_copy(&armature->bones.elements[i]));

    if (!armature->baked_ik) {
        struct skf_Vec_Bone inheritance_bones = {0};
        for (i = 0; i < armature->bones.size; i++)
            skf_Vec_append(inheritance_bones, skf_bone_shallow_copy(&armature->bones.elements[i]));
        skf_inheritance(&inheritance_bones, &ik_rots);
        ik_rots = skf_inverse_kinematics(&inheritance_bones, &armature->ik_root_ids);
        free(inheritance_bones.elements);
    }
    skf_inheritance(&bones, &ik_rots);
    skf_construct_verts(&bones);
    free(ik_rots.elements);
    return bones;
}

/**
 * @brief Searches a texture based off its name.
 * 
 * @param bone_texture Name of the texture to search for.
 * @param styles Pointer to the vector of the styles of the armature.
 * @return NULL if the texture is not found, else pointer to
 *  the texture.
 */
struct skf_Texture *skf_get_bone_texture(
    const char *bone_texture,
    const struct skf_Vec_Style *styles
)
{
    size_t style_i, texture_i;
    for (style_i = 0; style_i < styles->size; style_i++) {
        const struct skf_Style *style = &styles->elements[style_i];
        for (texture_i = 0; texture_i < style->textures.size; texture_i++) {
            struct skf_Texture *texture = &style->textures.elements[texture_i];
            if (strcmp(bone_texture, texture->name) == 0)
                return texture;
        }
    }
    return NULL;
}

/**
 * @brief Helper function to allow reversing or looping.
 * 
 * @param frame Frame number of the animation.
 * @param animation Pointer to the animation.
 * @param reverse skf_true if the returned frame should play the frames in reverse.
 * @param is_loop skf_true if the animation should loop back to the start when ended.
 * @return The new frame number to animate with.
 */
uint32_t skf_format_frame(
    uint32_t *frame,
    const struct skf_Animation *animation,
    const skf_bool reverse,
    const skf_bool is_loop
)
{
    uint32_t last_frame = (animation->keyframes.size > 0) ?
        (animation->keyframes.elements[animation->keyframes.size - 1].frame) :
        0;
    if (is_loop)
        *frame = *frame % (last_frame + 1);
    if (reverse)
        *frame = last_frame - *frame;
    return *frame;
}

/**
 * @brief Helper function to allow reversing or looping, using time instead of frames
 *  (and thus, framerate independent).
 * 
 * @param elapsed_time_seconds Elapsed time in seconds since start of the animation.
 * @param animation Pointer to the animation.
 * @param reverse skf_true if the returned frame should play the frames in reverse.
 * @param is_loop skf_true if the animation should loop back to the start when ended.
 * @return The new frame number to animate with.
 */
uint32_t skf_time_frame(
    const float elapsed_time_seconds,
    const struct skf_Animation *animation,
    const skf_bool reverse,
    const skf_bool is_loop
)
{
    const float frametime = 1.0f / animation->fps;
    uint32_t frame = elapsed_time_seconds / frametime;
    frame = skf_format_frame(&frame, animation, reverse, is_loop);
    return frame;
}

/**
 * @brief Corrects bone flipping, since one of the scale X/Y being negative
 *  mean that the rotation should be flipped.
 * 
 * @param bone Bone to have its rotation modified in-place.
 * @param scale The scale applied.
 */
void skf_check_bone_flip(
    struct skf_Bone *bone,
    const struct skf_Vec2 scale
)
{
    if ((scale.x < 0.0f) ^ (scale.y < 0.0f))
        bone->rot = -bone->rot;
}

#endif