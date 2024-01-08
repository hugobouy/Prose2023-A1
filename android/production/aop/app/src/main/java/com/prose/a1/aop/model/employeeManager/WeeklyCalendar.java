/**
 * @file 	WeeklyCalendar.java
 * @brief	Classe qui dessine et modifie le calendrier
 * @author 	CASSAR Bastien
 * @date 	15/04/2023
 *
 * \b License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @copyright Copyright (c) 2023 ProSE-A1-2024
 */

package com.prose.a1.aop.model.employeeManager;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.SurfaceView;

import com.prose.a1.aop.R;


public class WeeklyCalendar extends SurfaceView
{
    /*
     * LOCAL VARIABLES
     */
    /**
     * @brief index de la valeur max
     */
    private static final int MAX = 1;
    /**
     * @brief index de la valeur min
     */
    private static final int MIN = 0;
    /**
     * @brief nombre de jours en une semaine.
     */
    private static final int NB_DAYS = 7;
    /**
     * @brief Matrice stockant les informations du calendrier.
     */
    boolean[][] employeeSchedule = new boolean[48][7];
    /**
     * @brief Variable des jours de la semaine.
     */
    private static final String[] WEEK_DAYS = new String[] {" ", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam","Dim"};
    /**
     * @brief Variable pour les heures de la semaine.
     */
    private static final String[] HOURS = new String[] {" ", "00h00", "00h30", "01h00","01h30", "02h00","02h30", "03h00", "03h30","04h00","04h30", "05h00", "05h30","06h00","06h30","07h00","07h30",
            "08h00","08h30", "09h00","09h30", "10h00", "10h30","11h00","11h30", "12h00","12h30", "13h00","13h30", "14h00","14h30","15h00","15h30",
            "16h00","16h30", "17h00", "17h30","18h00", "18h30","19h00","19h30", "20h00", "20h30","21h00", "21h30","22h00","22h30","23h00","23h30"};
    /**
     * @brief Initialise le nombre d'heure en une journée plus un espace vide pour l'affichage.
     */
    int nbHours;
    /**
     * @brief Initialise le nombre de jour en une semaine plus un espace vide pour l'affichage.
     */
    int nbDays;
    /**
     * @brief Permet d'avoir la taille pour scroller vers le bas.
     */
    private static final int HEIGHT_PHONE = 1270;
    /**
     * @brief Variable permettant de dessiner les contours des cases.
     */
    private Paint paint;
    /**
     * @brief Variable permettant de colorier le fond des cases.
     */
    private Paint backgroundRect;
    /**
     * @brief Variable permettant d'écrire dans les cases.
     */
    private Paint textPaint;
    /**
     * @brief Variable permettant d'avoir la hauteur des cases.
     */
    private int cellHeight;
    /**
     * @brief Variable permettant de centrer le texte dans les cases.
     */
    private int textHeight;
    /**
     * @brief Variable permettant d'avoir la largeur des cases.
     */
    private int cellWidth;

    /*
     * FUNCTIONS
     */
    /**
     * @brief Variable permettant d'avoir la largeur des cases.
     * @param context [in] Permet d'avoir le context du fragment.
     * @param attrs [in] Permet de mettre à jour le nombre de jour et le nombre d'heure.
     */
    public WeeklyCalendar(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        TypedArray a = context.getTheme().obtainStyledAttributes(
                attrs,
                R.styleable.WeeklyCalendarView,
                0, 0);

        nbHours = a.getInt(R.styleable.WeeklyCalendarView_nbHours, 1);
        nbDays = a.getInt(R.styleable.WeeklyCalendarView_nbDays, 1);
        a.recycle();
    }
    /**
     * @brief Initialise les éléments pour dessiner le calendrier.
     * @param canvas [out] Permet de dessiner les formes du calendrier.
     */
    public void drawCalendar(Canvas canvas)
    {
        initBrush();
        initDim(canvas);
        // Dessine chaque semaine
        for (int i = 0; i < 1; i++)
        {
            // Dessine chaque jour de la semaine
            for (int j = 0; j < WEEK_DAYS.length; j++)
            {
                // Dessine la case du jour
                canvas.drawRect(myRect(j,i), paint);
                canvas.drawText(WEEK_DAYS[j], myRect(j,i).centerX(), myRect(j,i).centerY() + textHeight, textPaint);
            }
        }
        //Dessine chaque heure par tranche de 30 minutes
        for (int i = 0; i < 49; i++)
        {
            // Dessine chaque jour de la semaine
            for (int j = 0; j < 1; j++)
            {
                // Dessine la case du jour
                canvas.drawRect(myRect(j,i), paint);
                canvas.drawText(HOURS[i], myRect(j,i).centerX(), myRect(j,i).centerY() + textHeight, textPaint);
            }
        }

        //dessine les cases vide pour le calendrier
        for (int i = 1; i < 49; i++)
        {
            // Dessine chaque jour de la semaine
            for (int j = 1; j < 8; j++)
            {
                // Dessine la case du jour avec fond vert si les horaires correspondent sinon en blanc
                if (employeeSchedule[i-1][j-1])
                {
                    canvas.drawRect(myRect(j,i), backgroundRect);
                    canvas.drawRect(myRect(j,i), paint);
                }
                else
                {
                    canvas.drawRect(myRect(j,i), paint);
                }
            }
        }
    }
    /**
     * @brief Dessine les cases du calendrier.
     * @param abs [in] Modifie l'abscisse du rectangle selon son emplacement dans le calendrier.
     * @param ord [in] Modifie l'ordonnée du rectangle selon son emplacement dans le calendrier.
     * @return les cases.
     */
    private Rect myRect(int abs, int ord)
    {
        Rect rect;
        // Calcule les dimensions et la position de la case du jour
        int x = abs * cellWidth;
        int y = ord * cellHeight;
        rect = new Rect(x, y, x + cellWidth, y + cellHeight);
        return rect;
    }
    /**
     * @brief Remet la matrice à false.
     */
    private void cleanMatrix()
    {
        //remise de la matrice à false
        for (int i = 1; i < nbHours; i++)
        {
            // Dessine chaque jour de la semaine
            for (int j = 1; j < nbDays; j++)
            {
                employeeSchedule[i-1][j-1] = false;
            }
        }
    }
    /**
     * @brief Modifie la matrice selon le nom de l'employé.
     * @param name [in] Permet d'avoir le nom de l'employé.
     */
    public void editCalendar(String name)
    {
        cleanMatrix();
        int[][] index;
        index = CacheEmployeeManager.INSTANCE.askCalendarEmployee(name);
        for (int i = 0; i<NB_DAYS; i++)
        {
            for (int j = index[i][MIN]-1; j< index[i][MAX]-1; j++)
                employeeSchedule[j][i] = true;
        }
    }
    /**
     * @brief Initialise la hauteur et la largeur du téléphone.
     * @param canvas [in] Permet d'avoir les dimensions du téléphone.
     */
    private void initDim(Canvas canvas)
    {
        // Calcule les dimensions des cases de jour et la hauteur de l'en-tête
        cellWidth = canvas.getWidth() / WEEK_DAYS.length;
        cellHeight = canvas.getHeight() / nbHours;

        // Calcule la hauteur du texte pour centrer le texte dans la case de jour
        Rect textBounds = new Rect();
        textPaint.getTextBounds("0", 0, 1, textBounds);
        textHeight = textBounds.height();
    }
    /**
     * @brief Initialise les différents pinceaux pour dessiner les contours,
     * le fond et écrire le texte dans les cases.
     */
    private void initBrush()
    {
        // Initialise le pinceau pour colorier le fond (quadrillage)
        backgroundRect = new Paint(Paint.ANTI_ALIAS_FLAG);
        backgroundRect.setColor(Color.GREEN);
        backgroundRect.setStyle(Paint.Style.FILL);
        backgroundRect.setStrokeWidth(2);

        // Initialise le pinceau pour les cases de jour (quadrillage)
        paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(Color.BLACK);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeWidth(2);

        // Initialise le pinceau pour le texte
        textPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        textPaint.setColor(Color.BLACK);
        textPaint.setTextSize(24);
        textPaint.setTextAlign(Paint.Align.CENTER);
    }
    /**
     * @brief Dessine le calendrier
     * @param canvas le canvas sur lequel l'arrière-plan sera dessiné
     */
    @Override
    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        drawCalendar(canvas);
    }
    /**
     * @brief Permet de redimensionner le dessin.
     * @param widthMeasureSpec les exigences en matière d'espace horizontal imposées par le parent.
     *                         Les exigences sont codées avec
     *                         {@link android.view.View.MeasureSpec}.
     * @param heightMeasureSpec les exigences en matière d'espace vertical imposées par le parent.
     *                         Les exigences sont codées avec
     *                         {@link android.view.View.MeasureSpec}.
     *
     */
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = 2800+HEIGHT_PHONE;
        setMeasuredDimension(width, height);
    }
}
