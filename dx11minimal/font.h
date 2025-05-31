
#include <vector>
#include <wrl/client.h>

extern ID2D1Factory* d2dFactory;
extern ID2D1HwndRenderTarget* d2dRenderTarget;
extern ID2D1SolidColorBrush* d2dBrush;
std::vector<float> letter_01 = { 16409.33f,400.93f,16412.6f,126.78f,16267.9f,223.6f,16385.4f,308.46f };
std::vector<float> letter_02 = { 16074.25f,398.76f,16074.25f,135.48f,16218.95f,222.51f,16094.92f,298.67f };
std::vector<float> letter_03 = { 15919.77f,123.51f,16010.06f,220.34f,15936.09f,325.84f,15842.53f,220.34f,15908.89f,138.74f };
std::vector<float> letter_04 = { 15649.96f,325.84f,15646.7f,99.58f,15787.04f,187.7f,15787.04f,325.84f };
std::vector<float> letter_05 = { 15341.0f,325.84f,15341.f,176.82f,15407.35f,118.08f,15458.48f,175.73f,15535.73f,116.99f,15582.51f,176.82f,15582.51f,325.84f };
std::vector<float> letter_06 = { 15253.95f,53.89f,15253.95f,325.84f };
std::vector<float> letter_07 = { 15063.57f,52.8f,15063.57f,325.84f,15166.92f,108.28f,15122.31f,227.96f,15207.17f,325.84f };
std::vector<float> letter_08 = { 14929.76f,378.09f,14963.48f,400.93f,14990.68f,361.77f,14978.71f,131.13f };
std::vector<float> letter_09 = { 14900.38f,53.89f,14900.38f,325.84f };
std::vector<float> letter_10 = { 14811.16f,325.84f,14812.26f,167.03f,14761.12f,131.13f,14694.76f,172.47f,14688.24f,52.8f,14688.24f,325.84f };
std::vector<float> letter_11 = { 14593.59f,271.47f,14527.22f,314.99f,14455.42f,233.39f,14525.05f,134.39f,14606.64f,190.96f,14597.93f,359.6f,14535.92f,399.85f,14459.77f,349.81f };
std::vector<float> letter_12 = { 14442.36f,61.5f,14379.26f,75.65f,14379.26f,124.61f,14341.18f,138.74f,14430.4f,145.27f,14373.82f,156.15f,14378.17f,325.84f };
std::vector<float> letter_13 = { 14293.32f,264.95f,14210.63f,319.34f,14139.92f,225.78f,14217.16f,127.87f,14308.55f,229.04f,14166.03f,224.7f };
std::vector<float> letter_14 = { 14069.2f,57.15f,14077.91f,325.84f,13934.3f,222.51f,14051.8f,149.63f };
std::vector<float> letter_15 = { 13884.26f,184.44f,13815.72f,125.69f,13746.09f,214.9f,13815.72f,325.84f,13891.87f,260.59f };
std::vector<float> letter_16 = { 13544.82f,54.97f,13544.82f,325.84f,13700.4f,237.74f,13558.97f,156.15f };
std::vector<float> letter_17 = { 13327.24f,185.52f,13397.95f,131.13f,13462.14f,152.89f,13477.38f,325.84f,13326.15f,247.54f,13461.05f,215.99f };
std::vector<float> letter_18 = { 13210.83f,53.89f,13224.3f,74.79f,13242.38f,102.84f };
std::vector<float> letter_19 = { 12958.43f,388.96f,13067.46f,388.96f,13172.75f,388.96f };
std::vector<float> letter_20 = { 12802.86f,195.32f,12872.48f,51.71f,12927.97f,192.05f };
std::vector<float> letter_21 = { 12685.36f,66.94f,12751.73f,70.21f,12749.55f,387.89f,12694.06f,393.32f };
std::vector<float> letter_22 = { 12585.27f,52.8f,12622.46f,190.06f,12659.25f,325.84f };
std::vector<float> letter_23 = { 12562.42f,68.03f,12500.41f,68.03f,12513.47f,380.26f,12562.42f,388.96f };
std::vector<float> letter_24 = { 12258.89f,68.03f,12453.63f,73.47f,12249.1f,305.2f,12453.63f,325.84f };
std::vector<float> letter_25 = { 12007.58f,52.8f,12087.54f,115.9f,12104.4f,325.84f,12126.71f,125.69f,12214.29f,52.8f };
std::vector<float> letter_26 = { 11768.24f,53.89f,11841.84f,156.69f,11948.61f,51.23f,11759.53f,325.84f,11876.79f,200.5f,11962.97f,325.84f };
std::vector<float> letter_27 = { 11397.26f,52.8f,11463.62f,325.84f,11549.56f,49.53f,11633.34f,325.84f,11708.4f,53.89f };
std::vector<float> letter_28 = { 11149.21f,52.8f,11242.77f,325.84f,11355.91f,50.62f };
std::vector<float> letter_29 = { 10909.86f,72.38f,10896.44f,220.36f,11085.02f,338.92f,11077.4f,51.71f };
std::vector<float> letter_30 = { 10624.83f,66.94f,10840.23f,66.94f,10731.44f,89.79f,10731.44f,325.84f };
std::vector<float> letter_31 = { 10576.95f,101.76f,10483.39f,54.97f,10382.22f,143.1f,10579.14f,219.25f,10495.36f,322.6f,10397.45f,258.42f };
std::vector<float> letter_32 = { 10136.35f,325.84f,10138.52f,58.24f,10141.79f,58.24f,10332.17f,127.87f,10160.28f,194.23f,10353.93f,319.34f };
std::vector<float> letter_33 = { 9980.67f,303.04f,9897.f,314.99f,9831.73f,223.6f,9851.31f,99.58f,9956.84f,60.41f,10048.23f,139.84f,10026.47f,285.61f,9955.75f,259.51f,10073.25f,342.18f };
std::vector<float> letter_34 = { 9589.11f,325.84f,9594.56f,61.5f,9769.72f,132.22f,9628.3f,199.21f };
std::vector<float> letter_35 = { 9375.5f,70.26f,9287.76f,174.65f,9382.41f,325.84f,9495.55f,273.65f,9508.61f,150.71f,9397.64f,61.5f };
std::vector<float> letter_36 = { 9016.87f,318.25f,9020.13f,48.45f,9195.29f,325.84f,9206.17f,50.62f };
std::vector<float> letter_37 = { 8704.63f,319.34f,8707.9f,54.97f,8818.87f,325.84f,8925.49f,50.62f,8935.28f,325.84f };
std::vector<float> letter_38 = { 8490.31f,54.97f,8491.4f,313.9f,8644.79f,325.84f };
std::vector<float> letter_39 = { 8240.09f,56.06f,8240.09f,325.84f,8265.11f,217.07f,8396.75f,62.59f,8288.58f,220.47f,8414.16f,325.84f };
std::vector<float> letter_40 = { 8033.38f,247.54f,8062.76f,318.25f,8148.7f,294.32f,8158.49f,54.97f };
std::vector<float> letter_41 = { 7956.14f,51.71f,7959.41f,325.84f };
std::vector<float> letter_42 = { 7678.71f,56.06f,7678.71f,325.84f,7698.3f,194.23f,7841.91f,181.18f,7859.32f,62.59f,7856.06f,313.9f };
std::vector<float> letter_43 = { 7584.07f,127.87f,7473.1f,50.62f,7370.83f,186.62f,7487.24f,340.01f,7601.48f,205.11f,7501.39f,209.46f };
std::vector<float> letter_44 = { 7311.f,64.77f,7148.9f,69.12f,7152.16f,312.82f,7169.57f,187.7f,7288.15f,190.96f };
std::vector<float> letter_45 = { 7073.83f,68.03f,6892.15f,68.03f,7009.09f,193.14f,6896.5f,318.25f,7071.66f,309.56f };
std::vector<float> letter_46 = { 6620.17f,68.03f,6820.35f,106.1f,6622.34f,308.46f,6628.04f,145.36f };
std::vector<float> letter_47 = { 6535.31f,131.13f,6421.07f,53.89f,6337.3f,194.22f,6435.22f,328.04f,6536.39f,239.92f };
std::vector<float> letter_48 = { 6092.52f,316.08f,6095.79f,60.41f,6260.06f,114.81f,6128.42f,184.44f,6276.38f,245.36f,6107.75f,318.25f };
std::vector<float> letter_49 = { 5818.36f,317.16f,5924.98f,61.5f,6034.86f,325.84f,5879.29f,224.7f };
std::vector<float> letter_50 = { 5595.34f,289.96f,5517.01f,269.29f,5551.82f,158.33f,5616.01f,142.01f,5644.3f,301.93f,5689.98f,312.82f,5769.41f,206.2f,5606.22f,61.5f,5465.88f,124.61f,5451.73f,287.8f,5650.83f,386.79f,5772.67f,325.84f };
std::vector<float> letter_51 = { 5270.04f,119.16f,5324.45f,64.77f,5395.15f,135.48f,5315.75f,217.07f,5308.77f,286.96f,5327.71f,299.76f,5311.39f,325.84f,5291.81f,311.73f,5304.79f,296.31f };
std::vector<float> letter_52 = { 4736.96f,113.72f,4569.42f,192.05f,4723.91f,262.77f };
std::vector<float> letter_53 = { 1904.f,224.7f,1956.59f,225.32f,2004.09f,230.13f };
std::vector<float> letter_54 = { 1123.96f,58.24f,1129.12f,96.46f,1123.96f,143.1f };
std::vector<float> letter_55 = { 27.34f,54.99f,26.3f,247.f,26.04f,269.14f,41.89f,287.88f,6.3f,298.14f,26.45f,321.75f,45.83f,296.5f };
std::vector<float> letter_56 = { 169.86f,50.63f,152.12f,122.74f,108.64f,127.21f,235.31f,129.94f,250.04f,54.99f,245.13f,130.14f,275.66f,130.8f,244.97f,134.98f,227.13f,234.24f,232.55f,136.68f,148.78f,137.2f,125.68f,242.47f,72.83f,243.21f,258.73f,249.59f,228.75f,252.53f,216.78f,319.91f,222.35f,248.35f,121.48f,255.66f,108.65f,325.34f };
std::vector<float> letter_57 = { 457.05f,111.55f,429.86f,63.68f,391.98f,60.92f,383.26f,29.14f,384.12f,165.77f,372.2f,62.59f,323.24f,107.19f,355.88f,174.65f,442.92f,196.41f,469.03f,268.21f,399.4f,311.73f,389.82f,353.07f,382.2f,193.14f,380.03f,317.16f,353.7f,317.16f,321.07f,256.25f };
std::vector<float> letter_58 = { 689.91f,159.05f,744.73f,53.98f,667.88f,179.86f,627.86f,137.66f,627.86f,86.53f,580.f,56.06f,535.39f,85.44f,544.09f,165.94f,601.39f,183.51f,621.1f,155.56f,664.67f,195.38f,598.49f,325.84f,673.7f,199.39f,715.35f,242.15f,715.35f,293.28f,763.22f,323.75f,807.83f,294.37f,799.12f,213.87f,741.82f,196.3f,722.12f,224.25f,675.81f,186.06f,684.12f,170.13f };
std::vector<float> letter_59 = { 17943.31f,137.66f,18099.97f,133.31f,17933.52f,307.37f,18103.24f,314.99f };
std::vector<float> letter_60 = { 1042.37f,211.64f,955.34f,318.25f,874.82f,282.35f,886.79f,196.41f,1009.72f,130.04f,986.89f,61.5f,907.46f,75.65f,908.55f,142.01f,1061.95f,317.16f };
std::vector<float> letter_61 = { 1262.13f,50.62f,1194.67f,205.11f,1219.69f,325.84f,1257.77f,397.67f };
std::vector<float> letter_62 = { 1326.31f,46.28f,1381.8f,167.03f,1380.71f,307.37f,1331.75f,404.2f };
std::vector<float> letter_63 = { 17760.54f,386.79f,17805.14f,399.85f,17909.59f,128.95f,17793.18f,294.32f,17764.89f,127.87f };
std::vector<float> letter_64 = { 1834.37f,317.16f,1820.23f,292.14f,1852.87f,294.32f,1853.95f,345.45f,1821.33f,371.56f };
std::vector<float> letter_65 = { 2476.25f,135.48f,2571.99f,65.85f,2555.67f,325.84f };
std::vector<float> letter_66 = { 2885.31f,114.81f,2954.94f,53.89f,3027.83f,147.45f,2952.77f,184.44f,3028.37f,204.57f,2956.03f,321.51f,2889.66f,254.06f };
std::vector<float> letter_67 = { 2674.26f,128.95f,2733.f,61.5f,2820.03f,95.23f,2795.01f,195.32f,2682.96f,325.84f,2826.56f,313.9f };
std::vector<float> letter_68 = { 2408.8f,143.64f,2318.5f,66.94f,2249.97f,193.68f,2320.68f,325.84f,2394.66f,263.85f,2406.52f,175.81f };
std::vector<float> letter_69 = { 2127.03f,325.84f,2167.86f,178.14f,2210.79f,47.36f };
std::vector<float> letter_70 = { 17578.86f,126.78f,17625.53f,203.33f,17567.98f,325.84f,17707.23f,126.78f,17654.31f,244.94f,17717.02f,325.84f };
std::vector<float> letter_71 = { 17299.25f,128.95f,17353.66f,325.84f,17412.4f,126.78f,17470.07f,325.84f,17529.9f,127.87f };
std::vector<float> letter_72 = { 17105.6f,128.95f,17179.58f,325.84f,17253.57f,127.87f };
std::vector<float> letter_73 = { 16909.78f,128.95f,16917.62f,236.65f,17045.77f,325.84f,17044.68f,130.04f };
std::vector<float> letter_74 = { 1460.38f,103.46f,1432.93f,88.7f,1480.8f,102.84f,1486.24f,46.28f,1498.21f,98.49f,1539.54f,95.23f,1492.77f,110.46f,1517.79f,157.24f,1486.24f,119.16f,1461.21f,156.15f,1475.36f,113.72f };
std::vector<float> letter_75 = { 1674.45f,103.93f,1673.96f,181.98f,1592.6f,184.15f,1753.84f,186.33f,1673.22f,190.42f,1673.36f,276.91f };
std::vector<float> letter_76 = { 2051.38f,301.05f,2045.43f,308.46f,2067.19f,325.84f,2082.42f,304.11f,2057.33f,293.63f };
std::vector<float> letter_77 = { 16816.22f,71.3f,16821.66f,132.22f,16818.4f,132.22f,16781.4f,137.66f,16863.f,143.1f,16815.13f,153.98f,16818.4f,301.93f,16869.53f,325.84f };
std::vector<float> letter_78 = { 16605.16f,267.12f,16685.67f,325.84f,16747.68f,260.59f,16599.72f,186.61f,16674.78f,125.69f,16734.62f,179.f };
std::vector<float> letter_79 = { 3246.5f,197.49f,3074.61f,243.19f,3204.08f,64.77f,3212.78f,325.84f };
std::vector<float> letter_80 = { 16496.36f,325.84f,16495.28f,176.82f,16548.59f,125.69f,16570.35f,161.59f };
std::vector<float> letter_81 = { 3656.65f,116.99f,3606.61f,63.68f,3526.1f,110.46f,3526.1f,285.61f,3593.56f,325.84f,3676.23f,254.06f,3605.52f,158.34f,3540.25f,190.96f };
std::vector<float> letter_82 = { 3717.57f,72.38f,3874.24f,71.3f,3804.61f,190.96f,3776.32f,316.08f };
std::vector<float> letter_83 = { 3454.3f,70.21f,3333.21f,81.52f,3328.1f,182.26f,3471.32f,162.74f,3410.78f,314.99f,3316.13f,250.8f };
std::vector<float> letter_84 = { 3950.4f,125.69f,4015.43f,67.64f,4070.07f,135.48f,4023.58f,178.66f,4087.47f,237.74f,4008.78f,328.9f,3936.24f,229.04f,4000.79f,177.92f };
std::vector<float> letter_85 = { 4218.88f,320.67f,4305.16f,200.59f,4229.13f,60.69f,4148.97f,162.54f,4267.42f,216.45f };
std::vector<float> letter_86 = { 4386.65f,283.44f,4400.8f,308.46f,4383.39f,325.84f,4365.98f,300.84f,4386.65f,193.14f,4399.71f,149.63f,4382.3f,132.22f,4364.89f,157.24f,4381.95f,166.54f };
std::vector<float> letter_87 = { 5005.68f,112.63f,5177.58f,193.14f,5017.65f,264.95f };
std::vector<float> letter_88 = { 4486.74f,164.86f,4471.51f,140.92f,4494.36f,131.13f,4504.15f,146.36f,4486.35f,293.31f,4463.44f,296.04f,4478.22f,321.96f,4501.97f,293.23f,4497.62f,345.45f,4474.78f,369.39f };
std::vector<float> letter_89 = { 4780.98f,213.89f,4952.37f,215.75f,4786.73f,159.37f,4956.96f,158.86f };
std::vector<float> letter_90 = { 1123.96f,58.24f,1129.12f,96.46f,1123.96f,143.1f };
std::vector<float> letter_91 = { 0.f,0.f,0.f,0.f };

std::vector<float>* font[] = {
&letter_91,
&letter_55,
&letter_90,
&letter_56,
&letter_57,
&letter_58,
&letter_60,
&letter_54,
&letter_61,
&letter_62,
&letter_74,
&letter_75,
&letter_64,
&letter_53,
&letter_76,
&letter_69,
&letter_68,
&letter_65,
&letter_67,
&letter_66,
&letter_79,
&letter_83,
&letter_81,
&letter_82,
&letter_84,
&letter_85,
&letter_86,
&letter_88,
&letter_52,
&letter_89,
&letter_87,
&letter_51,
&letter_50,
&letter_49,//A
&letter_48,
&letter_47,
&letter_46,
&letter_45,
&letter_44,
&letter_43,
&letter_42,
&letter_41,
&letter_40,
&letter_39,
&letter_38,
&letter_37,
&letter_36,
&letter_35,
&letter_34,
&letter_33,
&letter_32,
&letter_31,
&letter_30,
&letter_29,
&letter_28,
&letter_27,
&letter_26,
&letter_25,
&letter_24,
&letter_23,
&letter_22,
&letter_21,
&letter_20,
&letter_19,
&letter_18,
&letter_17,
&letter_16,
&letter_15,
&letter_14,
&letter_13,
&letter_12,
&letter_11,
&letter_10,
&letter_09,
&letter_08,
&letter_07,
&letter_06,
&letter_05,
&letter_04,
&letter_03,
&letter_02,
&letter_01,
&letter_80,
&letter_78,
&letter_77,
&letter_73,
&letter_72,
&letter_71,
&letter_70,
&letter_63,
&letter_59 };

int letter_width[91];

bool font_preprocess_flag = false;

void preprocessFont()
{
    if (font_preprocess_flag) return;

    font_preprocess_flag = true;

    letter_width[0] = 14;
    int letters_count = sizeof(font) / sizeof(std::vector<float>*);

    for (int letter = 1; letter < letters_count; letter++)
    {
        //min-max
        float min_x = 1000000; float max_x = -100000;
        for (int i = 0; i < font[letter]->size(); i += 2)
        {
            float x = font[letter]->at(i);
            min_x = min(min_x, x);
            max_x = max(max_x, x);
        }

        //offset x
        for (int i = 0; i < font[letter]->size(); i += 2)
        {
            font[letter]->at(i) -= min_x;
        }

        //scale
        float scale = .1;

        for (int i = 0; i < font[letter]->size(); i++)
        {
            font[letter]->at(i) *= scale;
        }


        float width = max_x - min_x;

        letter_width[letter] = width * scale;

    }

}

float drawLetter(int letter, float x, float y, float scale, bool getSize = false)
{
    if (getSize)
    {
        return letter_width[letter] * scale;
    }

    if (letter == 0)
    {
        return letter_width[letter] * scale;
    }

    const std::vector<float>* pts = font[letter];
    int nPoints = (int)pts->size() / 2; // количество вершин


    for (int i = 0; i < nPoints; i++)
    {
        float px = x + pts->at(i * 2) * scale;
        float py = y + pts->at(i * 2 + 1) * scale;

        ConstBuf::global[i] = XMFLOAT4(px, py, 0.0f, 1.0f);
    }

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);

    Draw::NullDrawer(nPoints/6 , 1); 

    return letter_width[letter] * scale;
}

void letterProject(point3d& p)
{
    float camDist = 30;
    float x = p.x * camDist / (p.z + camDist);
    float y = p.y * camDist / (p.z + camDist);
    p.x = x;
    p.y = y;
}
point3d drawString(const char* str, float x, float y, float scale, bool centered, bool getSize = false, int count = -1)
{
    preprocessFont();

    scale = scale * window.width / 2560.;

    float tracking = 10;
    float interline = 40 * scale;

    //HPEN pen = CreatePen(PS_SOLID, 3, textStyle.color);
   // SelectObject(window.context, pen);

    int letters_count = strlen(str);
    if (count != -1) {
        letters_count = count;
    }
    float base_x = x;
    float base_y = y;
    int i = 0;
    float maxStringWidth = 0;
    float stringWidth = 0;

    while (i < letters_count)
    {
        float offset = 0;

        if (centered)
        {
            int j = i;
            while (j < letters_count && str[j] != '\n')
            {
                offset += letter_width[str[j] - 32] * scale + tracking;
                j++;
            }
            offset /= 2.;
        }

        while (i < letters_count && str[i] != '\n')
        {
            float sz = drawLetter(str[i] - 32, x - offset, y, scale, getSize) + tracking;
            x += sz;
            stringWidth += sz;
            i++;
        }

        maxStringWidth = max(maxStringWidth, stringWidth);

        i++;
        x = base_x;
        y += interline;
    }

   // DeleteObject(pen);

    return { maxStringWidth ,y - base_y,0 };

}


struct {
    COLORREF color;
} textStyle;



