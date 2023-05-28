#include <cstdio>
#include "camera.h"

Object *generate_walls()
{
    Texture *texture_bottom = new Texture("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/tile.ppm");
    Texture *texture_back = new Texture("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/back.ppm");
    Texture *texture_top = new Texture("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/top.ppm");
    TextureMap *color_back = new TextureMap(Vec3f(1.0f, 1.0f, 1.0f));
    TextureMap *color_front = new TextureMap(Vec3f(0.8, 0.2, 0.2));
    TextureMap *color_left = new TextureMap(Vec3f(0.2, 0.2, 0.8));
    TextureMap *color_right = new TextureMap(Vec3f(0.5, 0.8, 0.5));

    Object *walls = new Object;
    walls->vertexes_count = 8;
    walls->faces_count = 12;
    walls->vertexes = new Vec3f *[walls->vertexes_count]
    {
        new Vec3f(-1.2f, 1.0f, -2.5f),
            new Vec3f(-1.2f, 1.0f, 1.5f),
            new Vec3f(1.2f, 1.0f, 1.5f),
            new Vec3f(1.2f, 1.0f, -2.5f),
            new Vec3f(-1.2f, -1.0f, -2.5f),
            new Vec3f(-1.2f, -1.0f, 1.5f),
            new Vec3f(1.2f, -1.0f, 1.5f),
            new Vec3f(1.2f, -1.0f, -2.5f)
    };
    for (int i = 0; i < walls->vertexes_count; ++i)
    {
        *walls->vertexes[i] = *walls->vertexes[i] * 0.5;
    }
    Vec3f **vertexes = walls->vertexes;
    walls->faces = new Face *[walls->faces_count]
    {
        new Triangle(
            vertexes[1], vertexes[2], vertexes[5],
            new TextureMap(texture_back, 0, -1, 0, 0.5, 1 / 1.2, 0, 0, 0.5), MARBLE),
            new Triangle(
                vertexes[2], vertexes[5], vertexes[6],
                new TextureMap(texture_back, 0, -1, 0, 0.5, 1 / 1.2, 0, 0, 0.5), MARBLE),
            new Triangle(
                vertexes[0], vertexes[1], vertexes[2],
                new TextureMap(texture_top, 0, 0, 1.0 / 2, 2.5 / 4, 1.0 / 1.2, 0, 0, 0.5), DIFFUSE),
            new Triangle(
                vertexes[0], vertexes[2], vertexes[3],
                new TextureMap(texture_top, 0, 0, 1.0 / 2, 2.5 / 4, 1.0 / 1.2, 0, 0, 0.5), DIFFUSE),
            new Triangle(
                vertexes[4], vertexes[5], vertexes[6],
                new TextureMap(texture_bottom, 0, 0, 1. / 2, 2.5 / 4, 1. / 1.2, 0, 0, 0.5), FLOOR),
            new Triangle(
                vertexes[4], vertexes[6], vertexes[7],
                new TextureMap(texture_bottom, 0, 0, 1. / 2, 2.5 / 4, 1. / 1.2, 0, 0, 0.5), FLOOR),
            new Triangle(
                vertexes[0], vertexes[1], vertexes[4], color_left, WALL),
            new Triangle(
                vertexes[1], vertexes[4], vertexes[5], color_left, WALL),
            new Triangle(
                vertexes[2], vertexes[3], vertexes[6], color_right, WALL),
            new Triangle(
                vertexes[3], vertexes[6], vertexes[7], color_right, WALL),
            new Triangle(
                vertexes[0], vertexes[4], vertexes[7], color_front, WALL),
            new Triangle(
                vertexes[0], vertexes[3], vertexes[7], color_front, WALL)
    };
    return walls;
}

Object *generate_desk()
{
    Texture *texture_bottom = new Texture("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/bottom.ppm");
    TextureMap *color = new TextureMap(Vec3f(1.0f, 1.0f, 1.0f));
    Object *desk = new Object;
    desk->vertexes_count = 4;
    desk->faces_count = 2;
    const double theta = 45 / 180.0 * PI;
    desk->vertexes = new Vec3f *[desk->vertexes_count]
    {
        new Vec3f(-10, 0 - 10 * sin(theta), -10 * cos(theta)),
            new Vec3f(-10, 0 + 10 * sin(theta), 10 * cos(theta)),
            new Vec3f(10, 0 + 10 * sin(theta), 10 * cos(theta)),
            new Vec3f(10, 0 - 10 * sin(theta), -10 * cos(theta))
    };
    Vec3f **vertexes = desk->vertexes;
    desk->faces = new Face *[desk->faces_count]
    {
        new Triangle(
            vertexes[0], vertexes[1], vertexes[2], color, DESK),
            new Triangle(
                vertexes[0], vertexes[2], vertexes[3], color, DESK)
    };
    return desk;
}

Object *generate_light(Vec3f p, double r)
{
    Object *light = new Object;
    light->faces_count = 1;
    light->faces = new Face *[1]
    {
        new Disc(p, r, new TextureMap(Vec3f(1.0f, 1.0f, 1.0f)), LIGHT)
    };
    return light;
}

Scene *sceneBox()
{
    Object *bunny = new Object;
    bunny->import_ply("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/bunny.ply", new TextureMap(Vec3f(0.8, 0.7, 0.6)), STANFORD_MODEL);
    // printf("faq scenebox\n");
    bunny->scale(2.8, 0, 0, 0.24,
                 0, 2.8, 0, -0.09 - 0.5,
                 0, 0, -2.8, 0.12);
    bunny->rotate(30 * PI / 180.0);

    // Object *dragon = new Object;
    // bunny->import_ply("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/dragon_vrip.ply", new TextureMap(Vec3f(0.4, 0.8, 0.8)), STANFORD_MODEL);
    // // printf("faq scenebox\n");
    // bunny->scale(2.8, 0, 0, 0.24,
    //              0, 2.8, 0, -0.09 - 0.5,
    //              0, 0, -2.8, 0.12);
    // bunny->rotate(15 * PI / 180.0);

    Object *water = new Object;
    water->import_ply("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/water.ply", new TextureMap(Vec3f(1.0f, 1.0f, 1.0f)), WATER);
    water->scale(
        1. / 5.52799 * 1.2, 0, 0, -0.6,
        0, 0.12 / (1.85354 - 1.34492), 0, -0.31731 + 0.08,
        0, 0, 1.2 / (5.59200 + 0.00456), -1.19902 + 0.75);
    water->center = new Vec3f(0.0f, -1.0f, 0.0f);
    water->print_box();

    // Object *teapot = new Object;
    // teapot->import_bpt("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/teapot.bpt", new TextureMap(Vec3f(0.1, 0.8, 0.8)), LIGHT);
    // teapot->scale(
    //     -0.02, 0, 0, 0,
    //     0, 0, 0.02, 0,
    //     0, 0.02, 0, 0);
    // teapot->print_box();

    Scene *scene = new Scene(Vec3f(0.0, 0.5 - 1e-5, 0.1), 0.2, Vec3f(0.0f, -1.0f, 0.0f));
    // scene->add_object(dragon);
    scene->add_object(bunny);
    scene->add_object(water);
    scene->add_object(generate_walls());
    scene->add_object(generate_light(Vec3f(0.0f, 0.5 - 1e-6, 0.1f), 0.2));
    scene->add_object(new Sphere(
        Vec3f(-0.32, -0.30, 0.3), 0.18, new TextureMap(Vec3f(1.0f, 1.0f, 1.0f)), GLASS));
    scene->add_object(new Sphere(
        Vec3f(0.42, 0.20, 0), 0.15, new TextureMap(Vec3f(1.0f, 1.0f, 1.0f)), MIRROR));
    // scene->add_object(new Sphere(
    //     Vec3f(-0.42, -0.15, -0.1), 0.15, new TextureMap(Vec3f(7.0f, 7.0f, 7.0f)), GLASS));
    return scene;
}

Scene *scene_teapot()
{
    Object *water = new Object;
    water->import_ply("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/water.ply", new TextureMap(Vec3f(1.0f, 1.0f, 1.0f)), WATER);
    water->scale(
        1. / 5.52799 * 1.2, 0, 0, -0.6,
        0, 0.12 / (1.85354 - 1.34492), 0, -0.31731 + -0.05,
        0, 0, 1.2 / (5.59200 + 0.00456), -1.19902 + 0.75);
    water->center = new Vec3f(0.0f, -1.0f, 0.0f);
    water->print_box();

    Object *teapot = new Object;
    // teapot->import_bpt("D:/work/InSchool/cg/cg_project_group19/cg_project_group19_code/data/teapot.bpt", new TextureMap(Vec3f(0.1, 0.8, 0.8)), LIGHT);
    // teapot->scale(
    //     -0.08, 0, 0, 0.02,
    //     0, 0, 0.08, -0.05,
    //     0, 0.08, 0, 0.1);
    // teapot->print_box();

    Scene *scene = new Scene(Vec3f(0.0, 0.5 - 1e-5, 0.1), 0.2, Vec3f(0.0f, -1.0f, 0.0f));
    scene->add_object(generate_walls());
    scene->add_object(teapot);
    scene->add_object(water);
    scene->add_object(generate_light(Vec3f(0.0f, 0.5 - 1e-6, 0.1f), 0.2));
    return scene;
}

int main(int argc, char *argv[])
{
    // main function
    // printf("faq main\n");
    // Scene *scene = scene_teapot();
    Scene *scene = sceneBox();
    // Camera *camera = new Camera(1024, 768);
    Camera *camera = new Camera(800, 600);
    // Camera *camera = new Camera(80, 60);
    camera->set_scene(scene);
    camera->set_position(Vec3f(0, 0.15, -1));
    camera->set_length(0.684, 0.811, 1e-3, 1.09);
    /*if(argc > 2)
    {
        camera->render(stoi(argv[1]), stoi(argv[2]));
    }
    else
    {
        camera->render(stoi(argv[1]));
    }*/
    // camera->render(100, 200000);
    camera->render(600, 200000);
    camera->save_pic("result.ppm");
    return 0;
}