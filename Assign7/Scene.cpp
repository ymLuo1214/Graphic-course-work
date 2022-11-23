//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <iostream>
void Scene::buildBVH()
{
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        if (objects[k]->hasEmit())
        {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum)
            {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
    const Ray &ray,
    const std::vector<Object *> &objects,
    float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k)
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear)
        {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }

    return (*hitObject != nullptr);
}

Vector3f Scene::shade(Intersection &inter, Vector3f wo) const
{
    if (inter.m->hasEmission())
    {
        return inter.m->getEmission();
    }
    const float eps = 0.0005f;

    float light_p = 0.0;
    Intersection light_inter;
    sampleLight(light_inter, light_p);
    
    Vector3f L_dir;
    Vector3f dir = light_inter.coords - inter.coords;
    Ray pl(inter.coords, normalize(dir));
    Intersection block = intersect(pl);
    if (block.distance > -eps + dir.norm())
    {
        float dot1 = std::max(.0f, dotProduct(pl.direction, inter.normal));
        float dot2 = std::max(.0f, dotProduct(-pl.direction, light_inter.normal));
        Vector3f f_r=inter.m->eval(pl.direction,wo,inter.normal);
        L_dir = light_inter.emit * f_r* dot1 * dot2 / dotProduct(dir,dir)/ light_p;
    }

    Vector3f L_indir;
    if (get_random_float() < RussianRoulette)
    {
        Vector3f wi = inter.m->sample(wo, inter.normal).normalized();
        if (inter.m->pdf(wi, wo, inter.normal) > eps)
        {
            Ray pq(inter.coords, wi);
            Intersection obj = intersect(pq);
            if (obj.happened == true && !obj.m->hasEmission())
            {
                float dot3 = std::max(0.f, dotProduct(wi, inter.normal));
                L_indir = shade(obj, -wi) * inter.m->eval(wi, wo, inter.normal) * dot3 / inter.m->pdf(wi, wo, inter.normal) / RussianRoulette;
            }
        }
    }

    return L_indir + L_dir;
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    Vector3f res;
    Intersection inter = intersect(ray);
    if (inter.happened == false)
        return res;
    res = shade(inter, -ray.direction);
    return res;
}