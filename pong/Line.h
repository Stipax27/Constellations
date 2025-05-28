
void drawLine(point3d& p1, point3d& p2, int count)
{
    if (count < 1) return;

    for (int i = 0;i < count;i++)
    {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float dz = p2.z - p1.z;
        float length = sqrt(dx * dx + dy * dy + dz * dz);

        point3d point;
        point.x = p1.x + dx * (float)i / (float)(count - 1);
        point.y = p1.y + dy * (float)i / (float)(count - 1);
        point.z = p1.z + dz * (float)i / (float)(count - 1);

        if (modelProject)
        {
            modelProject(point);
        }
        float sz = 1 + .5 * sinf(i + currentTime * .01);
        point.draw(point, sz);
        // Рисование Линий.
    }
}
