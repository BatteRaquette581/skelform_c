#ifndef SKELFORM_C_H
#define SKELFORM_C_H

#include <math.h>
#include <memory.h>
#include <stdbool.h> 
#include <stdint.h>

#define Vec_BASE_CAPACITY 8
#define Vec(T) struct Vec_##T { \
    T *elements;                \
    size_t capacity;            \
    size_t size;                \
}
#define Vec_struct(T) struct Vec_##T { \
    struct T *elements;                \
    size_t capacity;                   \
    size_t size;                       \
}
#define Vec_append(vec, element) {                                                  \
    if (vec.capacity <= vec.size) {                                                 \
        if (vec.capacity == 0)                                                      \
            vec.capacity = Vec_BASE_CAPACITY;                                       \
        else                                                                        \
            vec.capacity <<= 1;                                                     \
        vec.elements = realloc(vec.elements, vec.capacity * sizeof(*vec.elements)); \
    }                                                                               \
    vec.elements[vec.size++] = element;                                             \
}
Vec(int32_t);
Vec(uint32_t);

struct Vec2 {
    float x;
    float y;
};

bool Vec2_eq(struct Vec2 v1, struct Vec2 v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

struct Vertex {
    struct Vec2 pos;
    struct Vec2 uv;
    struct Vec2 init_pos;
};
Vec_struct(Vertex);

enum HandlePreset {
    HandlePreset_Linear,
    HandlePreset_SineIn,
    HandlePreset_SineOut,
    HandlePreset_SineInOut,
    HandlePreset_None,
    HandlePreset_Custom
};

struct Keyframe {
    struct Vec2 start_handle;
    struct Vec2 end_handle;
    const char *element;
    const char *value_str;
    int32_t frame;
    int32_t bone_id;
    float value;
    float label_top;
    enum HandlePreset handle_preset;
};
Vec_struct(Keyframe);

struct Animation {
    struct Vec_Keyframe keyframes;
    const char *name;
    int32_t fps;
};
Vec_struct(Animation);

struct BoneBindVert {
    int32_t id;
    float weight;
};
Vec_struct(BoneBindVert);

struct BoneBind {
    struct Vec_BoneBindVert verts;
    int32_t bone_id;
    bool is_path;
};
Vec_struct(BoneBind);

struct Bone {
    struct Vec_int32_t ik_bone_ids;
    struct Vec_Vertex vertices;
    struct Vec_uint32_t indices;
    struct Vec_BoneBind binds;
    struct Vec2 scale;
    struct Vec2 pos;
    struct Vec2 init_scale;
    struct Vec2 init_pos;
    const char *name;
    const char *tex;
    const char *ik_constraint;
    const char *ik_mode;
    const char *init_ik_constraint;
    int32_t id;
    int32_t parent_id;
    int32_t ik_family_id;
    int32_t ik_target_id;
    float rot;
    float zindex;
    float init_rot;
};
Vec_struct(Bone);

struct Texture {
    struct Vec2 offset;
    struct Vec2 size;
    const char *name;
    int32_t atlas_idx;
};
Vec_struct(Texture);

struct Style {
    struct Vec_Texture textures;
    const char *name;
    int32_t id;
    bool active;
};
Vec_struct(Style);

struct TexAtlas {
    struct Vec2 size;
    const char *filename;
};
Vec_struct(TexAtlas);

struct Metadata {
    size_t last_anim;
    int32_t last_frame;
};

struct Armature {
    struct Vec_int32_t ik_root_ids;
    struct Vec_Bone bones;
    struct Vec_Animation animations;
    struct Vec_Texture textures;
    struct Vec_Style styles;
    struct Vec_TexAtlas atlases;
    struct Metadata metadata;
    bool baked_ik;
};


size_t get_prev_frame(
    const struct Vec_Keyframe *keyframes,
    const int32_t frame, 
    const char *element,
    const int32_t id
)
{
    size_t i, prev = SIZE_MAX;
    for (i = 0; i < keyframes->size; i++) {
        const struct Keyframe *kf = &keyframes->elements[i];
        if (kf->frame <= frame && kf->element == element && kf->bone_id == id)
            prev = i;
    }
    return prev;
}

size_t get_next_frame(
    const struct Vec_Keyframe *keyframes,
    const int32_t frame, 
    const char *element,
    const int32_t id
)
{
    size_t i;
    for (i = 0; i < keyframes->size; i++) {
        const struct Keyframe *kf = &keyframes->elements[i];
        if (kf->frame > frame && kf->element == element && kf->bone_id == id)
            return i;
    }
    return SIZE_MAX;
}

float cubic_bezier(const float t, const float p1, const float p2)
{
    const float u = 1.0f - t;
    return 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t;
}

float cubic_bezier_derivative(const float t, const float p1, const float p2)
{
    const float u = 1.0f - t;
    return 3.0f * u * u * p1 + 6.0f * u * t * (p2 - p1) + 3.0f * t * t * (1.0f - p2);
}

/* https://stackoverflow.com/a/16659263 */
float clamp(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

#define NEWTON_RAPHSON_EPSILON 0.00001f
float interpolate(
    const int32_t current,
    const int32_t max,
    const float start_val,
    const float end_val,
    const struct Vec2 *start_handle,
    const struct Vec2 *end_handle
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
            const float x = cubic_bezier(t, start_handle->x, end_handle->x);
            const float dx = cubic_bezier_derivative(t, start_handle->x, end_handle->x);
            if (fabsf(dx) < NEWTON_RAPHSON_EPSILON)
                break;
            t -= (x - initial) / dx;
            t = clamp(t, 0.0f, 1.0f);
        }

        {
            const float progress = cubic_bezier(t, start_handle->y, end_handle->y);
            return start_val + (end_val - start_val) * progress;
        }
    }
}

/* Interpolate an f32 value from the specified keyframe data. */
void interpolate_keyframes(
    const char *element,
    float *field,
    const struct Vec_Keyframe *keyframes,
    const int32_t id,
    const int32_t frame,
    const int32_t blend_frames
)
{
    size_t prev = get_prev_frame(keyframes, frame, element, id);
    size_t next = get_next_frame(keyframes, frame, element, id);

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
        const int32_t total_frames = keyframes->elements[next].frame - keyframes->elements[prev].frame;
        const int32_t current_frame = frame - keyframes->elements[prev].frame;

        const float result = interpolate(
            current_frame,
            total_frames,
            keyframes->elements[prev].value,
            keyframes->elements[next].value,
            &keyframes->elements[next].start_handle,
            &keyframes->elements[next].end_handle
        );

        const struct Vec2 z = {0.0f, 0.0f};
        *field = interpolate(current_frame, blend_frames, *field, result, &z, &z);
    }
}

void interpolate_bone(
    struct Bone *bone,
    const struct Vec_Keyframe *keyframes,
    const int32_t bone_id,
    const int32_t frame,
    const int32_t blend_frame
)
{
    size_t prev_frame;
    interpolate_keyframes(
        "PositionX",
        &bone->pos.x,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    interpolate_keyframes(
        "PositionY",
        &bone->pos.y,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    interpolate_keyframes(
        "Rotation",
        &bone->rot,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    interpolate_keyframes(
        "ScaleX",
        &bone->scale.x,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    interpolate_keyframes(
        "ScaleY",
        &bone->scale.y,
        keyframes,
        bone_id,
        frame,
        blend_frame
    );
    prev_frame = get_prev_frame(keyframes, frame, "IkConstraint", bone->id);
    if (prev_frame != SIZE_MAX) {
        bone->ik_constraint = keyframes->elements[prev_frame].value_str;
    }

    prev_frame = get_prev_frame(keyframes, frame, "Texture", bone->id);
    if (prev_frame != SIZE_MAX) {
        bone->ik_constraint = keyframes->elements[prev_frame].value_str;
    }
}

bool is_animated(
    const int32_t bone_id,
    const char *el, 
    const struct Vec_Animation *anims
)
{
    size_t anim_i, kf_i;
    for (anim_i = 0; anim_i < anims->size; anim_i++) {
        const struct Animation *anim = &anims->elements[anim_i];
        for (kf_i = 0; kf_i < anim->keyframes.size; kf_i++) {
            const struct Keyframe *kf = &anim->keyframes.elements[kf_i];
            if (kf->bone_id == bone_id && kf->element == el)
                return true;
        }
    }
    return false;
}

void reset_bone(
    struct Bone *bone,
    const int32_t frame,
    const int32_t blend_frame,
    const struct Vec_Animation *anims
)
{
    const struct Vec2 z = {0.0f, 0.0f};
    if (!is_animated(bone->id, "PositionX", anims))
        bone->pos.x = interpolate(frame, blend_frame, bone->pos.x, bone->init_pos.x, &z, &z);
    if (!is_animated(bone->id, "PositionY", anims))
        bone->pos.y = interpolate(frame, blend_frame, bone->pos.y, bone->init_pos.y, &z, &z);
    if (!is_animated(bone->id, "Rotation", anims))
        bone->rot = interpolate(frame, blend_frame, bone->rot, bone->init_rot, &z, &z);
    if (!is_animated(bone->id, "ScaleX", anims))
        bone->scale.x = interpolate(frame, blend_frame, bone->scale.x, bone->init_scale.x, &z, &z);
    if (!is_animated(bone->id, "ScaleY", anims))
        bone->scale.y = interpolate(frame, blend_frame, bone->scale.y, bone->init_scale.y, &z, &z);
    if (!is_animated(bone->id, "IkConstraint", anims))
        bone->ik_constraint = bone->init_ik_constraint;
}

void animate(
    struct Vec_Bone *bones,
    const struct Vec_Animation *anims,
    const struct Vec_int32_t *frames,
    const struct Vec_int32_t *blend_frames
)
{
    size_t a, b, bone_i;
    for (a = 0; a < anims->size; a++) {
        for (b = 0; b < bones->size; b++) {
            interpolate_bone(
                &bones->elements[b],
                &anims->elements[a].keyframes,
                bones->elements[b].id,
                frames->elements[a],
                blend_frames->elements[a]
            );
        }
    }

    for (bone_i = 0; bone_i < bones->size; bone_i++) {
        reset_bone(
            &bones->elements[bone_i],
            frames->elements[0],
            blend_frames->elements[0],
            anims
        );
    }
}

#endif