#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <stdlib.h>
using namespace std;

#define INFINITE 1073741823
#define PI 3.1415926535897

int n,a[20][20];

vector<vector<pair<int, int> > > graph;

vector<int> path;

int from, to;

ALLEGRO_FONT* af;
ifstream is("test.txt");

void costdrum()
{
    for(int k=0; k<n; k++)
        for(int i=0; i<n; i++)
            for(int j=0; j<n; j++)
                if(a[i][j]>a[i][k]+a[k][j]) a[i][j]=a[i][k]+a[k][j];
}
void Load()
{
    int m;
    is >> n;

    graph.resize(n);

    is >> m;
    for(int i=0; i<n; i++)
        for(int j=0; j<n; j++)
            if(i==j) a[i][j]=0;
            else a[i][j]=INFINITE;

    for (int i = 0; i<m; i++)
    {
        int x, y, v;
        is >> x >> y >> v;
        a[x][y]=v;
        graph[x].push_back(pair<int, int>(y, v) );
    }
    costdrum();
}


void drawArrow(int i, int j, bool c)
{
    ALLEGRO_COLOR color = al_map_rgb(c ? 0 : 255, c ? 255 : 0, 0);

    float x1 = 240 + 200 * sin(i * 2 * PI / n);
    float y1 = 240 + 200 * cos(i * 2 * PI / n);
    float x2 = 240 + 200 * sin(j * 2 * PI / n);
    float y2 = 240 + 200 * cos(j * 2 * PI / n);

    float x11 = (x1 * 9 + x2) / 10;
    float y11 = (y1 * 9 + y2) / 10;
    float x21 = (x2 * 9 + x1) / 10;
    float y21 = (y2 * 9 + y1) / 10;

    float pivx = (x2 * 4 + x1) / 5;
    float pivy = (y2 * 4 + y1) / 5;

    float dx = (x21 - pivx) / 5;
    float dy = (y21 - pivy) / 5;

    al_draw_line(x11, y11, x21, y21, color, 2);

    al_draw_filled_triangle(x21, y21, pivx + dy, pivy - dx, pivx - dy, pivy + dx, color);
}

void drawCost(int i, int j, int c)
{
    float x1 = 235 + 200 * sin(i * 2 * PI / n);
    float y1 = 235 + 200 * cos(i * 2 * PI / n);
    float x2 = 235 + 200 * sin(j * 2 * PI / n);
    float y2 = 235 + 200 * cos(j * 2 * PI / n);

    x1 = (x1 + x2) / 2;
    y1 = (y1 + y2) / 2;

    char buff[16];
    memset(buff, 0, sizeof(buff));
    itoa ( c, buff, 10 );
    al_draw_text(af, al_map_rgb(0, 255 , 255), x1+10, y1+10, 0, buff);
}

void drawCost1(int i, int j, int c)
{
    float x1 = 235 + 200 * sin(i * 2 * PI / n);
    float y1 = 235 + 200 * cos(i * 2 * PI / n);
    float x2 = 235 + 200 * sin(j * 2 * PI / n);
    float y2 = 235 + 200 * cos(j * 2 * PI / n);

    x1 = (x1 + x2) / 2;
    y1 = (y1 + y2) / 2;

    char buff[16];
    memset(buff, 0, sizeof(buff));
    itoa ( c, buff, 10 );
    al_draw_text(af, al_map_rgb(0, 255 , 255), x1+15, y1-30, 0, buff);
}

void drawCost2(int i, int j, int c)
{
    float x1 = 235 + 200 * sin(i * 2 * PI / n);
    float y1 = 235 + 200 * cos(i * 2 * PI / n);
    float x2 = 235 + 200 * sin(j * 2 * PI / n);
    float y2 = 235 + 200 * cos(j * 2 * PI / n);

    x1 = (x1 + x2) / 2;
    y1 = (y1 + y2) / 2;

    char buff[16];
    memset(buff, 0, sizeof(buff));
    itoa ( c, buff, 10 );
    al_draw_text(af, al_map_rgb(0, 255 , 255), x1-15, y1+30, 0, buff);
}

void drawNode(int i)
{
    float x1 = 235 + 200 * sin(i * 2 * PI / n);
    float y1 = 235 + 200 * cos(i * 2 * PI / n);

    char buff[16];
    memset(buff, 0, sizeof(buff));
    itoa ( i, buff, 10 );

    al_draw_text(af, al_map_rgb(255,255,0), x1, y1, 0, buff);
}

void draw()
{
    for (int i = 0; i < n; i++)
    {
        drawNode(i);
        for (int j = 0; j < graph[i].size(); j++)
        {
            drawArrow(i, graph[i][j].first, false);
            if(a[i][graph[i][j].first]!=INFINITE && a[graph[i][j].first][i]!=INFINITE)
            {
                if(i<graph[i][j].first) drawCost1(i, graph[i][j].first, graph[i][j].second);
                else drawCost2(i, graph[i][j].first, graph[i][j].second);
            }
            else drawCost(i, graph[i][j].first, graph[i][j].second);
        }
    }
}

struct Details
{
    bool visited;
    int cost;
    int prev;

    Details() :
        visited(false),
        cost(INFINITE),
        prev(-1)
    {

    }
};

void ComputeDijkstra()
{
    vector<Details> d;
    d.resize(n);

    int minim = from;
    d[minim].cost = 0;
    while (minim != to && minim != -1)
    {
        d[minim].visited = true;
        for (int i = 0; i<graph[minim].size(); i++)
        {
            if (d[graph[minim][i].first].visited == false)
            {
                if (d[graph[minim][i].first].cost > d[minim].cost + graph[minim][i].second)
                {
                    d[graph[minim][i].first].cost = d[minim].cost + graph[minim][i].second;
                    d[graph[minim][i].first].prev = minim;
                }
            }
        }
        minim = -1;
        int minv = INFINITE;
        for (int i = 0; i<n; ++i)
        {
            if (d[i].visited == false && minv > d[i].cost)
            {
                minv = d[i].cost;
                minim = i;
            }
        }
    }

    if (minim == to)
    {
        path.push_back(minim);

        while (minim != from)
        {
            minim = d[minim].prev;
            path.push_back(minim);
        }
    }
}

void drawPath()
{
    draw();
    for (int i = path.size() - 1; i > 0; --i)
    {
        drawArrow(path[i], path[i-1], true);
        cout << path[i] << " ";
    }
    cout << path[0] << endl;
}

int main()
{
    int width = 640;
    int height = 480;

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;

    if (!al_init())
        return -1;

    display = al_create_display(width, height);

    if (!display)
        return -1;

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_ttf_addon();

    af = al_load_ttf_font("Times New Roman.ttf", 21, 0);

    event_queue = al_create_event_queue();

    Load();
    draw();
    al_flip_display();

    cout <<"Se porneste de la nodul: ";
    cin>>from;
    cout<<endl<<"Se ajunge in nodul: ";
    cin>>to;
    if(from>=n || to>=n) cout<<"Eroare";
    else if(a[from][to]!=INFINITE)
    {
        ComputeDijkstra();
        cout<<endl<<endl<<"Drumul este: ";
        drawPath();
        al_flip_display();

        al_rest(1.0);
        al_register_event_source(event_queue, al_get_keyboard_event_source());

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
    }
    else cout<<"Nu exista drum intre cele 2 noduri";
    return 0;
}
