#include <stdio.h>
#include <stdlib.h>

// Ogrenci yapisi, her ogrencinin numarasini, adini, soyadini ve bolumunu tutar
struct Student {
    int number;  // Ogrencinin numarasi
    char name[20];  // Ogrencinin adi
    char surname[20];  // Ogrencinin soyadi
    char department[50];  // Ogrencinin bolumu
};

// Indeks yapisi, ogrenci numarasi ve ilgili dosya offset'ini tutar
struct Index {
    int number;  // Ogrencinin numarasi
    long offset;  // Ogrencinin dosyada bulundugu yerin offset degeri
};

// Dosya olusturulurken her ogrenciye default degerler atama islemi
void createFile() {
    FILE *file = fopen("students.txt", "wb");
    if (file == NULL) {
        printf("File could not be opened.\n");
        return;
    }
    struct Student student = {0, "", "", ""};  // Ogrenciyi baslangic degerleriyle doldur
    for (int i = 0; i < 100; i++) {
        fwrite(&student, sizeof(struct Student), 1, file);  // Dosyaya 100 ogrenci ekle
    }
    printf("File created successfully.\n");
    fclose(file);
}

// Indeks dosyasi olusturulmasi ve her ogrenciye offset degeri atanmasi
void createIndex() {
    FILE *file = fopen("students.txt", "rb");
    if (file == NULL) {
        printf("File could not be opened.\n");
        return;
    }

    FILE *indexFile = fopen("index.txt", "wb");
    if (indexFile == NULL) {
        printf("Index file could not be created.\n");
        fclose(file);
        return;
    }

    struct Index index[100] = {0};  // 100 ogrenci icin indeks dizisi

    for (int i = 0; i < 100; i++) {
        index[i].number = i + 1;  // Ogrenci numarasini 1'den 100'e kadar ata
        index[i].offset = i * sizeof(struct Student);  // Her ogrenciye dosya offset'ini ata
        fwrite(&index[i], sizeof(struct Index), 1, indexFile);  // Indeksi dosyaya yaz
    }

    printf("Index file created successfully.\n");

    fclose(file);
    fclose(indexFile);
}

// Ogrenci bilgilerini bulma islemi. Ogrenci numarasina gore dosyaya erisim saglanir
void findStudent() {
    int stuNum;
    printf("Enter student number to find(1-100): ");
    scanf("%d", &stuNum);  // Ogrenci numarasini al

    // Indeks dosyasini ac
    FILE *indexFile = fopen("index.txt", "rb");
    if (indexFile == NULL) {
        printf("Index file could not be opened.\n");
        return;
    }

    struct Index index;
    fseek(indexFile, (stuNum - 1) * sizeof(struct Index), SEEK_SET);  // Belirtilen numaraya git
    fread(&index, sizeof(struct Index), 1, indexFile);  // Indeks bilgisini oku

    // Ogrenci bulunamadiysa bilgi ver
    if (index.number == 0) {
        printf("Student not found.\n");
        fclose(indexFile);
        return;
    }

    // Ogrencinin dosya offset'ini kullanarak, ogrencinin bilgilerine ulasilir
    FILE *file = fopen("students.txt", "rb");
    if (file == NULL) {
        printf("File could not be opened.\n");
        fclose(indexFile);
        return;
    }

    struct Student student;
    fseek(file, index.offset, SEEK_SET);  // Offset'e git
    fread(&student, sizeof(struct Student), 1, file);  // Ogrenci bilgilerini oku

    // Ogrenci bilgilerini ekrana yazdir
    printf("Student found:\n");
    printf("Number: %d\nName: %s\nSurname: %s\nDepartment: %s\n", 
        student.number, student.name, student.surname, student.department);

    fclose(file);
    fclose(indexFile);
}

// Yeni ogrenci ekleme islemi
void addStudent() {
    // Dosyayi okuma ve yazma modunda ac
    FILE *file = fopen("students.txt", "rb+");
    if (file == NULL) {
        printf("File could not be opened.\n");
        return;
    }

    struct Student student;
    printf("Enter student number(1-100): ");
    int stuNum;
    scanf("%d", &stuNum);  // Ogrenci numarasini al

    // Ogrenci numarasinin gecerli olup olmadigini kontrol et
    if (stuNum < 1 || stuNum > 100) {
        printf("Invalid number.\n");
        fclose(file);
        return;
    }

    fseek(file, (stuNum - 1) * sizeof(struct Student), SEEK_SET);  // Ogrenci numarasina uygun konuma git
    fread(&student, sizeof(struct Student), 1, file);  // Ogrenci verilerini oku

    // Ogrenci zaten varsa, uyarı ver
    if (student.number != 0) {
        printf("Student already exists.\n");
        fclose(file);
        return;
    }

    // Yeni ogrenci bilgilerini al
    printf("Enter student name: ");
    scanf("%s", student.name);
    printf("Enter student surname: ");
    scanf("%s", student.surname);
    printf("Enter student department: ");
    scanf("%s", student.department);
    student.number = stuNum;  // Ogrenci numarasini ata

    fseek(file, (stuNum - 1) * sizeof(struct Student), SEEK_SET);  // Dosyada uygun yere git
    fwrite(&student, sizeof(struct Student), 1, file);  // Ogrenci bilgilerini dosyaya yaz

    fclose(file);
    printf("Student data written to file successfully\n");

    updateIndex(stuNum, (stuNum - 1) * sizeof(struct Student));  // Indeks dosyasini guncelle
}

// Ogrenci eklenince, indeks dosyasini guncelleme islemi
void updateIndex(int stuNum, long offset) {
    // Indeks dosyasini okuma ve yazma modunda ac
    FILE *indexFile = fopen("index.txt", "rb+");
    if (indexFile == NULL) {
        printf("Index file could not be opened.\n");
        return;
    }

    struct Index index;
    fseek(indexFile, (stuNum - 1) * sizeof(struct Index), SEEK_SET);  // Indeks dosyasinda uygun yere git
    fread(&index, sizeof(struct Index), 1, indexFile);  // Ogrenci indeks bilgisini oku

    // Ogrenci zaten indeks dosyasinda varsa, guncellemeyi yapma
    if (index.number != 0) {
        printf("Student already exists in index.\n");
        fclose(indexFile);
        return;
    }

    index.number = stuNum;  // Ogrenci numarasini indeks dosyasina yaz
    index.offset = offset;  // Ogrenci offset bilgisini indeks dosyasina yaz

    fseek(indexFile, (stuNum - 1) * sizeof(struct Index), SEEK_SET);  // Indeks dosyasinda uygun yere git
    fwrite(&index, sizeof(struct Index), 1, indexFile);  // Indeksi dosyaya yaz

    fclose(indexFile);
    printf("Index updated successfully.\n");
}

int main(void) {
    int choice;

    while (1) {
        printf("\n1. Create new file\n2. Add new student\n3. Find student\n4. Exit\n");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createFile();
                break;
            case 2:
                addStudent();
                break;
            case 3: {
                findStudent();
                break;
            }
            case 4:
                printf("Exiting program...\n");  // Programi sonlandir
                return 0;
            default:
                printf("Invalid choice. Please enter a valid option.\n");  // Gecersiz secim
        }
    }

    return 0;
}
